#include "escuchar_cpu_kernel.h"

//pthread_mutex_t mutex_READY;
pthread_mutex_t mutex_BLOCKED;

void escuchar_mensajes_dispatch_kernel(){
    bool desconexion_dispatch_kernel = 0;
	while(!desconexion_dispatch_kernel){
		int cod_op = recibir_operacion(fd_cpu_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_pcb* pcb;
			case FIN_DE_QUANTUM:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_EXEC);
				pthread_mutex_lock(&mutex_READY);
				queue_push(READY, pcb);
				pthread_mutex_unlock(&mutex_READY);
				pcb->estado = E_READY;
				break;

			case ENTRADA_SALIDA:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_EXEC);
				sem_post(&sem_desalojo);
				pthread_mutex_lock(&mutex_BLOCKED);
				list_add(BLOCKED, pcb);
				pthread_mutex_unlock(&mutex_BLOCKED);
				pcb->estado = E_BLOCKED;
				break;
			case ELIMINAR_PROCESO:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_EXEC);
				sem_post(&sem_MULTIPROGRAMACION);
				pcb->estado = E_EXIT;
				// hacer cosas de memoria
				break;

			case -1:
				log_error(logger_kernel, "El Dispatch se desconecto de Kernel. Terminando servidor.");
				desconexion_dispatch_kernel = 1;
				break;
			default:
				log_warning(logger_kernel, "Operacion desconocida de Dispatch-Kernel.");
				log_info(logger_kernel, "Cop: %d", cod_op);
				break;
			}
	}
}


void escuchar_mensajes_interrupt_kernel(){
    bool desconexion_interrupt_kernel = 0;
	while(!desconexion_interrupt_kernel){
		int motivo = recibir_operacion(fd_cpu_interrupt); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(motivo){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_kernel, "El Interrupt se desconecto de Kernel. Terminando servidor.");
				desconexion_interrupt_kernel = 1;
				break;
			default:
				log_warning(logger_kernel, "Operacion desconocida de Interrupt-Kernel.");
				break;
			}
	}
}


/*
1) ENTRADA/SALIDA
2) QUANTUM DE RR Y VRR
3) HACER VRR E INTERRUPCION DE QUANTUM
4) HACER RECURSOS
5) ELIMINAR PROCESO (TERMINAR LARGO PLAZO)
6) SHARED


*/


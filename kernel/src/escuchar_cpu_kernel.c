#include "escuchar_cpu_kernel.h"

//pthread_mutex_t mutex_READY;
pthread_mutex_t mutex_BLOCKED;


void escuchar_mensajes_dispatch_kernel(){
    bool desconexion_dispatch_kernel = 0;
	while(!desconexion_dispatch_kernel){
		int cod_op = recibir_operacion(fd_cpu_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_pcb* pcb;
			int pid;
			t_buffer* buffer;
			case FIN_DE_QUANTUM:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_EXEC);

				pcb->quantum = quantum;

				pthread_mutex_lock(&mutex_READY);
				list_add(READY, pcb);
				pthread_mutex_unlock(&mutex_READY);
				pcb->estado = E_READY;
				break;

			case ENTRADA_SALIDA:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_desalojo);
				pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?
				pthread_mutex_lock(&mutex_BLOCKED);
				list_add(BLOCKED, pcb);
				pthread_mutex_unlock(&mutex_BLOCKED);
				pcb->estado = E_BLOCKED;
				sem_post(&sem_EXEC);
				break;
			case ELIMINAR_PROCESO:
				pcb = recibir_pcb(fd_cpu_dispatch); //chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_desalojo);
				sem_post(&sem_EXEC);
				//sem_post(&sem_MULTIPROGRAMACION); lo hacemos en eliminar_proceso()
			
				pcb->estado = E_EXIT;
				pthread_mutex_lock(&mutex_EXIT);
				list_add(EXIT, pcb);
				pthread_mutex_unlock(&mutex_EXIT);
				
				sem_post(&sem_EXIT);
				// hacer cosas de memoria
				break;
			case KERNEL_WAIT: //pónernos de acuerdo con nacho como envia el recurso solicitado
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				
				sem_post(&sem_desalojo);
				pcb->quantum = RUNNING->quantum;
				
				// RECURSOS = ["RA", "RB", "RC"];
				char* recurso_solicitado = extraer_string_del_buffer(buffer); // "RB"

				if (buscar_recurso(recurso_solicitado) < 0){
					enviar_a_exit(RUNNING);
				} else {
					restar_instancia(recursos_disponibles, pcb);
				}

				sem_post(&sem_EXEC);
				
				destruir_buffer(buffer);
				break;
			case KERNEL_SIGNAL:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				char* recurso = extraer_string_del_buffer(buffer);

				sem_post(&sem_desalojo);
				pcb->quantum = RUNNING->quantum;

				sumar_instancia(recurso, pcb);

				destruir_buffer(buffer);
				
				sem_post(&sem_EXEC);
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
/* 
WAIT (Recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro.
SIGNAL (Recurso): Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro.
RECURSOS=[RA,RB,RC]
INSTANCIAS_RECURSOS=[1,2,1]
*/


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
2) HACER RECURSOS
3) CONSOLA
4) QUANTUM DE RR Y VRR (hecho)
5) HACER VRR E INTERRUPCION DE QUANTUM (hecho)
6) ELIMINAR PROCESO (TERMINAR LARGO PLAZO) (hecho)



*/


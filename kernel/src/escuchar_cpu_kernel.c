#include "escuchar_cpu_kernel.h"

void escuchar_mensajes_dispatch_kernel(){
    bool desconexion_dispatch_kernel = 0;
	while(!desconexion_dispatch_kernel){
		int cod_op = recibir_operacion(fd_cpu_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;			
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
		int cod_op = recibir_operacion(fd_cpu_interrupt); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case CPU_INTERRUPT:
				motivo_interrupcion motivo = recibir_operacion(fd_cpu_interrupt);
				t_pcb* pcb = recibir_pcb(fd_cpu_interrupt);
				if (motivo == FIN_DE_QUANTUM){
					sem_post(&sem_EXEC);
					queue_push(READY, pcb);
				}
				else if(motivo == ENTRADA_SALIDA){
					list_add(BLOCKED, pcb);
					// hacer cosas de IO
				}
				else if(motivo == ELIMINAR_PROCESO){
					sem_post(&sem_MULTIPROGRAMACION);
					// hacer cosas de memoria
				}
				break;
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
#include "escuchar_entradasalida_kernel.h"


pthread_mutex_t mutex_PRIORIDAD;



void escuchar_mensajes_entradasalida_kernel(){
    bool desconexion_entradasalida_kernel = 0;
	while(!desconexion_entradasalida_kernel){
		int cod_op = recibir_operacion(fd_entradasalida); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_kernel, fd_entradasalida, cod_op);
				break;
			case FIN_IO:
				int pid = recibir_int(fd_entradasalida);
				int index = buscar_index_por_pid(BLOCKED, pid);
				t_pcb* pcb = list_get(BLOCKED, index);
				list_remove_and_destroy_element(BLOCKED, index, pcb_destruir);

				if (pcb->quantum < quantum){
					pcb->estado = E_PRIORIDAD;
                	pthread_mutex_lock(&mutex_PRIORIDAD);
					queue_push(PRIORIDAD, pcb);
					pthread_mutex_unlock(&mutex_PRIORIDAD);
				} else {
					pcb->estado = E_READY;
                	pthread_mutex_lock(&mutex_READY);
					queue_push(READY, pcb);
					pthread_mutex_unlock(&mutex_READY);
				}
				break;
			case -1:
				log_error(logger_kernel, "La Entradasalida se desconecto de Kernel. Terminando servidor.");
				desconexion_entradasalida_kernel = 1;
				break;
			default:
				log_warning(logger_kernel, "Operacion desconocida de Entradasalida-Kernel.");
				break;
			}
	}
}
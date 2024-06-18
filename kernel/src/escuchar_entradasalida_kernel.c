#include "escuchar_entradasalida_kernel.h"


pthread_mutex_t mutex_PRIORIDAD;



void escuchar_mensajes_entradasalida_kernel(int indice_interfaz){
    bool desconexion_entradasalida_kernel = 0;

	t_interfaz* interfaz = list_get(interfaces, indice_interfaz);
	int socket = interfaz->socket;

	sem_t* sem_ocupado;
	sem_init(sem_ocupado, 1, 1);

	// HILO LISTA DE ESPERA
	pthread_t hilo_lista_espera;
	pthread_create(hilo_lista_espera, NULL, esperar_entradasalida, (sem_ocupado, indice_interfaz)); // Valgrind?
	pthread_detach(hilo_lista_espera);

	while(!desconexion_entradasalida_kernel){
		int cod_op = recibir_operacion(socket); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			int pid;
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_kernel, socket, cod_op);
				break;
			case FIN_IO:
				pid = recibir_int(socket);
				int index = buscar_index_por_pid(BLOCKED, pid);
				t_pcb* pcb = list_get(BLOCKED, index);
				list_remove_and_destroy_element(BLOCKED, index, pcb_destruir);

				if (pcb->quantum < quantum){
					pcb->estado = E_PRIORIDAD;
                	pthread_mutex_lock(&mutex_PRIORIDAD);
					list_add(PRIORIDAD, pcb);
					pthread_mutex_unlock(&mutex_PRIORIDAD);
				} else {
					pcb->estado = E_READY;
                	pthread_mutex_lock(&mutex_READY);
					list_add(READY, pcb);
					pthread_mutex_unlock(&mutex_READY);
				}
				
				sem_post(&sem_ocupado);
				break;
			case ERROR_IO:
				t_buffer* buffer = recibir_buffer_completo(socket);
				pid = extraer_int_del_buffer(buffer);

				index = buscar_index_por_pid(BLOCKED, pid);
        		pthread_mutex_lock(&mutex_BLOCKED);
        		pcb = list_remove(BLOCKED, index);
				pthread_mutex_unlock(&mutex_BLOCKED);
				

				pcb->estado = E_EXIT;
				pthread_mutex_lock(&mutex_EXIT);
				list_add(EXIT, pcb);
				pthread_mutex_unlock(&mutex_EXIT);

				sem_post(&sem_ocupado);
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

void esperar_entradasalida(sem_t sem_ocupado, int indice){

	t_interfaz* interfaz = list_get(interfaces, indice);
	sem_wait(&sem_ocupado);
	sem_wait(&interfaz->sem_espera);

	pthread_mutex_lock(&(interfaz->mutex_interfaz));
	t_paquete* paquete = list_remove(interfaz->cola_espera, 0);
	pthread_mutex_unlock(&(interfaz->mutex_interfaz));

	enviar_paquete(paquete, interfaz->socket);
}
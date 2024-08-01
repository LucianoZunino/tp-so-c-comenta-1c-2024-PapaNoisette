#include "escuchar_entradasalida_kernel.h"

pthread_mutex_t mutex_PRIORIDAD;
sem_t sem_ocupado;

void escuchar_mensajes_entradasalida_kernel(int indice_interfaz){
//void escuchar_mensajes_entradasalida_kernel(){
    bool desconexion_entradasalida_kernel = 0;

	printf("Indice interfaz: %i\n", indice_interfaz);
	printf("Length interfaces: %i\n", list_size(interfaces));
	if (list_size(interfaces) == 0) return;
	t_interfaz* interfaz = list_get(interfaces, indice_interfaz);
	int socket = *interfaz->socket;

	sem_init(&sem_ocupado, 1, 1);
	 // HILO LISTA DE ESPERA
	pthread_t hilo_lista_espera;
	pthread_create(&hilo_lista_espera, NULL, esperar_entradasalida, indice_interfaz); // Valgrind?
	pthread_detach(hilo_lista_espera);
	 
	// Mensaje ES semaforo datos
	//enviar_ok(NUEVA_CONEXION_IO, socket);

	while(!desconexion_entradasalida_kernel){
		int cod_op = recibir_operacion(socket); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){

			t_buffer* buffer;
			int pid;

			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_kernel, socket, cod_op);
				break;
			case FIN_IO:
				printf("\n\n\nLLEGUE A FIN_IO\n\n\n");
				buffer = recibir_buffer_completo(socket);
				pid = extraer_int_del_buffer(buffer);
				//pid = recibir_int(socket); QUE MIERDA ES ESTA FUNCIÓN?
				printf("\n\n\nRECIBIDO_BUFFER\n\n\n");
				int index = buscar_index_por_pid(BLOCKED, pid);
				printf("\n\n\nPOST_INDEX\n\n\n");

				t_pcb* pcb = list_remove(BLOCKED, index);				

				printf("\n\nDESPUES DE ELIMINAR DE BLOCKED\n\n\n");
				if(pcb->quantum < quantum){
					pcb->estado = E_PRIORIDAD;
                	pthread_mutex_lock(&mutex_PRIORIDAD);
					list_add(PRIORIDAD, pcb);
					pthread_mutex_unlock(&mutex_PRIORIDAD);
					cambio_de_estado(pcb, E_PRIORIDAD);
				}
				else{
					//pcb->estado = E_READY;
                	pthread_mutex_lock(&mutex_READY);
					list_add(READY, pcb);
					pthread_mutex_unlock(&mutex_READY);
					cambio_de_estado(pcb, E_READY);
					sem_post(&sem_READY);
				}
				
				printf("\n\n\n PASA LOS IF DE FIN_IO\n\n\n");
				sem_post(&sem_ocupado);

				break;
			case ERROR_IO:
				buffer = recibir_buffer_completo(socket);
				pid = extraer_int_del_buffer(buffer);

				index = buscar_index_por_pid(BLOCKED, pid);
        		pthread_mutex_lock(&mutex_BLOCKED);
        		pcb = list_remove(BLOCKED, index);
				pthread_mutex_unlock(&mutex_BLOCKED);
				
				enviar_a_exit(pcb, "INVALID_INTERFACE");

				sem_post(&sem_ocupado);
				break;
			case NUEVA_CONEXION_IO:
				buffer = recibir_buffer_completo(socket);

				char* nombre = extraer_string_del_buffer(buffer);

				t_interfaz* interfaz = list_get(interfaces, indice_interfaz);  // Verificar que list_get modifica la lista

				memcpy(interfaz->nombre, nombre, strlen(nombre));

				printf("Conectada interfaz: %s\n", nombre);

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

void esperar_entradasalida(int indice){
	t_interfaz* interfaz = list_get(interfaces, indice);

	while(true){
		sem_wait(&sem_ocupado);
		sem_wait(&interfaz->sem_espera);

		pthread_mutex_lock(&(interfaz->mutex_interfaz));
		t_paquete* paquete = list_remove(interfaz->cola_espera, 0);
		pthread_mutex_unlock(&(interfaz->mutex_interfaz));

		printf("\n\nCodigo de operacion instruccion IO:%i\n\n", paquete->codigo_operacion);
		enviar_paquete(paquete, *interfaz->socket);
		eliminar_paquete(paquete); //Si rompe es esto
	}
}
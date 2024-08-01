#include "escuchar_cpu_kernel.h"

// pthread_mutex_t mutex_READY;
pthread_mutex_t mutex_BLOCKED;

void escuchar_mensajes_dispatch_kernel(){
	bool desconexion_dispatch_kernel = 0;

	while(!desconexion_dispatch_kernel){
		int cod_op = recibir_operacion(fd_cpu_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		printf("\n\n\nCOD_OP DESDE E.CPU ANTES DEL SWITCH: %i\n\n\n", cod_op);

		switch(cod_op){
			t_pcb *pcb;
			int pid;
			int tiempo;
			int registro_tamanio;
			int registro_direccion;
			int indice_interfaz;
			char *nombre_interfaz;
			char* nombre_archivo;
			int tamanio;
			int dir_logica;
			int dir_fisica;
			int puntero_archivo;
			t_interfaz *interfaz;
			t_buffer *buffer;
			t_buffer* buffer1;
			t_paquete *paquete;
			t_paquete *paquete1;

			printf("\n\n\nCOD_OP DESDE E.CPU: %i\n\n\n", cod_op);
			
			case FIN_DE_QUANTUM:
				validar_desalojo();
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer); // chequear si anda, sino usar deserealizar_pcb
				sem_post(&sem_EXEC);

				pcb->quantum = quantum;

				log_info(logger_kernel, "PID: %i - Desalojado por fin de Quantum", pcb->pid);

				pthread_mutex_lock(&mutex_READY);
				list_add(READY, pcb);
				pthread_mutex_unlock(&mutex_READY);
				cambio_de_estado(pcb, E_READY);
				//pcb->estado = E_READY;

				destruir_buffer(buffer);

				break;
			case IO_GEN_SLEEP_FS:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 // chequear si anda, sino usar deserealizar_pcb
				validar_desalojo();
				//sem_post(&sem_desalojo);
				//bloquear_proceso(pcb, interfaz->nombre);
	
				nombre_interfaz = extraer_string_del_buffer(buffer);
				tiempo = extraer_int_del_buffer(buffer);
				log_info(logger_kernel, " Se recibio solicitud de sleep para la interfaz: %s por el tiempo:%d",nombre_interfaz,tiempo);
			
				indice_interfaz = buscar_interfaz(nombre_interfaz);

				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}

				interfaz = list_get(interfaces, indice_interfaz);

				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				destruir_buffer(buffer);


				buffer1 = crear_buffer();
				
				cargar_int_al_buffer(buffer1, pcb->pid);
				cargar_int_al_buffer(buffer1, tiempo);

				paquete1 = crear_paquete(IO_GEN_SLEEP_FS, buffer1); // aca no iria generica?

				pthread_mutex_lock(&interfaz->mutex_interfaz);

				list_add(interfaz->cola_espera, paquete1);

				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);	

				bloquear_proceso(pcb, interfaz->nombre);

		

				break;
			case IO_STDIN_READ_FS:

				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?

				nombre_interfaz = extraer_string_del_buffer(buffer);
				registro_direccion = extraer_int_del_buffer(buffer);
				registro_tamanio = extraer_int_del_buffer(buffer);


				validar_desalojo();
				
				
				indice_interfaz = buscar_interfaz(nombre_interfaz);
				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}
				
				interfaz = list_get(interfaces, indice_interfaz);

				destruir_buffer(buffer);
				
				buffer1 = crear_buffer();
				paquete1 = crear_paquete(IO_STDIN_READ_FS, buffer1);
				cargar_int_al_buffer(paquete1->buffer, pcb->pid);
				cargar_int_al_buffer(paquete1->buffer, registro_direccion);
				cargar_int_al_buffer(paquete1->buffer, registro_tamanio);



				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete1);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);

				
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				bloquear_proceso(pcb, interfaz->nombre);


				break;
			case IO_STDOUT_WRITE_FS:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	

				nombre_interfaz = extraer_string_del_buffer(buffer);
				registro_direccion = extraer_int_del_buffer(buffer);
				registro_tamanio = extraer_int_del_buffer(buffer);

				validar_desalojo();//sem_post(&sem_desalojo);

				indice_interfaz = buscar_interfaz(nombre_interfaz);

				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}

				interfaz = list_get(interfaces, indice_interfaz);

				destruir_buffer(buffer);

				buffer = crear_buffer();
				paquete = crear_paquete(IO_STDOUT_WRITE_FS, buffer);
				cargar_int_al_buffer(paquete->buffer,pcb->pid);
				cargar_int_al_buffer(paquete->buffer, registro_direccion);
				cargar_int_al_buffer(paquete->buffer, registro_tamanio);

				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);

				
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				bloquear_proceso(pcb, interfaz->nombre);
				break;
			case IO_FS_READ_FS:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				nombre_interfaz = extraer_string_del_buffer(buffer);
				nombre_archivo = extraer_string_del_buffer(buffer);
				dir_logica = extraer_int_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);
				puntero_archivo = extraer_int_del_buffer(buffer);

				validar_desalojo();//sem_post(&sem_desalojo);
				indice_interfaz = buscar_interfaz(nombre_interfaz);

				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}

				interfaz = list_get(interfaces, indice_interfaz);
				destruir_buffer(buffer);
				buffer = crear_buffer();
				paquete = crear_paquete(IO_FS_READ_FS, buffer);
				cargar_int_al_buffer(paquete->buffer, pcb->pid);
				cargar_string_al_buffer(paquete->buffer, nombre_archivo);
				cargar_int_al_buffer(paquete->buffer, dir_logica);
				cargar_int_al_buffer(paquete->buffer, tamanio);
				cargar_int_al_buffer(paquete->buffer, puntero_archivo);
				
				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);
				sem_post(&interfaz->sem_espera);
				
				
				sem_post(&sem_EXEC);
				bloquear_proceso(pcb, interfaz->nombre);

				break;
			case IO_FS_WRITE_FS:
				
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				nombre_interfaz = extraer_string_del_buffer(buffer);
				nombre_archivo = extraer_string_del_buffer(buffer);
				dir_logica = extraer_int_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);
				puntero_archivo = extraer_int_del_buffer(buffer);
				
				validar_desalojo();
				indice_interfaz = buscar_interfaz(nombre_interfaz);

				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}
				
				interfaz = list_get(interfaces, indice_interfaz);

				destruir_buffer(buffer);
				buffer = crear_buffer();

				paquete = crear_paquete(IO_FS_WRITE_FS, buffer);
				cargar_int_al_buffer(paquete->buffer, pcb->pid);
				cargar_string_al_buffer(paquete->buffer, nombre_archivo);
				cargar_int_al_buffer(paquete->buffer, dir_logica);
				cargar_int_al_buffer(paquete->buffer, tamanio);
				cargar_int_al_buffer(paquete->buffer, puntero_archivo);
				
				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);
				
				
				//sem_post(&sem_desalojo);
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				bloquear_proceso(pcb, interfaz->nombre);

				break;
			case IO_FS_TRUNCATE_FS:
				
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				nombre_interfaz = extraer_string_del_buffer(buffer);
				nombre_archivo = extraer_string_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);

				printf("\nLLEGA DE CPU REGISTRO ECX: %i\n", pcb->registros_cpu->ECX);
				validar_desalojo(); //sem_post(&sem_desalojo);

				indice_interfaz = buscar_interfaz(nombre_interfaz);
				
				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					
					break;
				}
				
				interfaz = list_get(interfaces, indice_interfaz);
				
				destruir_buffer(buffer);
				buffer = crear_buffer();
				
				paquete = crear_paquete(IO_FS_TRUNCATE_FS, buffer);
				cargar_int_al_buffer(paquete->buffer, pcb->pid);
				cargar_string_al_buffer(paquete->buffer, nombre_archivo);
				cargar_int_al_buffer(paquete->buffer, tamanio);
				
				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);
				
				sem_post(&interfaz->sem_espera);
				
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				bloquear_proceso(pcb, interfaz->nombre);

				break;
			case IO_FS_CREATE_FS:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				nombre_interfaz = extraer_string_del_buffer(buffer);
				nombre_archivo = extraer_string_del_buffer(buffer);
				validar_desalojo(); // sem_post(&sem_desalojo);

				printf("\n LLEGA DE CPU DI: %i\n ", pcb->registros_cpu->DI);
				
				indice_interfaz = buscar_interfaz(nombre_interfaz);
				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}
				interfaz = list_get(interfaces, indice_interfaz);
				
				paquete = crear_paquete(IO_FS_CREATE_FS, buffer);
				cargar_int_al_buffer(paquete->buffer, pcb->pid);
				cargar_string_al_buffer(paquete->buffer, nombre_archivo);

				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);

				
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);
				bloquear_proceso(pcb, interfaz->nombre);
				

				break;
			case IO_FS_DELETE_FS:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);	 
				nombre_interfaz = extraer_string_del_buffer(buffer);
				nombre_archivo = extraer_string_del_buffer(buffer);
				validar_desalojo();
				

				indice_interfaz = buscar_interfaz(nombre_interfaz);
				if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
					break;
				}
				interfaz = list_get(interfaces, indice_interfaz);
				
				destruir_buffer(buffer);
				buffer = crear_buffer();

				paquete = crear_paquete(IO_FS_DELETE_FS, buffer);
				cargar_int_al_buffer(paquete->buffer, pcb->pid);
				cargar_string_al_buffer(paquete->buffer, nombre_archivo);

				pthread_mutex_lock(&interfaz->mutex_interfaz);
				list_add(interfaz->cola_espera, paquete);
				pthread_mutex_unlock(&interfaz->mutex_interfaz);

				sem_post(&interfaz->sem_espera);

				
				bloquear_proceso(pcb, interfaz->nombre);
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);
				
				//destruir_buffer(buffer);

				break;
			// case ELIMINAR_PROCESO:
			case KERNEL_EXIT:
				validar_desalojo();
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				
				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);
				
				enviar_a_exit(pcb, "SUCCESS");
				destruir_buffer(buffer);
				// CONTEMPLAR EL CASO PARA DEVOLVER RECURSOS SI LOS TIENE

				break;
			case KERNEL_WAIT: // ponernos de acuerdo con nacho como envia el recurso solicitado
				printf("\n INICIAR KERNEL WAIT\n");
				validar_desalojo();//sem_post(&sem_desalojo)
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				printf("\n ANTES DEL RECURSO_SOLICITADO\n");
				// RECURSOS = ["RA", "RB", "RC"];
				char *recurso_solicitado = extraer_string_del_buffer(buffer); // "RB"
				printf("\n ANTES DE VALIDAR_DESALOJO\n");

				;
				pcb->quantum = RUNNING->quantum;
				
				usleep(20);
				//usleep(retardo_respuesta);
				
				printf("\n ANTES DEL IF \n");
				if(buscar_recurso(recurso_solicitado) < 0){
					printf("\nADENTRO DEL IF BUSCAR RECURSO ANTES DE ENVIAR A EXIT\n");
					enviar_a_exit(pcb, "INVALID_RESOURCE");
					printf("\nADENTRO DEL IF BUSCAR RECURSO DESPUES DE ENVIAR A EXIT\n");
				}
				else{
					printf("\n ANTES DEL RESTAR INSTANCIA\n");
					restar_instancia(recurso_solicitado, pcb);
					printf("\n DESP DEL RESTAR INSTANCIA\n");
				}
				
				printf("\n ANTES DEL SEM_EXEC \n");
				
				sem_post(&sem_EXEC);
				
				printf("\n ANTES DEL DESTRUIR BUFFER \n");
				destruir_buffer(buffer);
				printf("\n FIN WAIT \n");

				break;
			case KERNEL_SIGNAL:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				char *recurso = extraer_string_del_buffer(buffer);

				validar_desalojo();//sem_post(&sem_desalojo);

				printf("\n ####### ANTES DE SUMAR INSTANCIA A: %s ######\n", recurso);
				sumar_instancia(recurso, pcb);
				printf("\n ####### DESP DE SUMAR INSTANCIA ######\n");
				

				pcb->quantum = RUNNING->quantum;
				sem_post(&sem_EXEC);

				if(pcb->estado == E_EXIT){
					break;
				}
				
				if(pcb->quantum != quantum){ // Devuelvo el proceso original a la cola de ready o de prioridad, segun corresponda
                    pthread_mutex_lock(&mutex_PRIORIDAD);
			        list_add(PRIORIDAD, pcb);
			        pthread_mutex_unlock(&mutex_PRIORIDAD);
                    cambio_de_estado(pcb, E_PRIORIDAD);
                    sem_post(&sem_READY);
                }else{
					pthread_mutex_lock(&mutex_READY);
			    	list_add(READY, pcb);
			    	pthread_mutex_unlock(&mutex_READY);
                	cambio_de_estado(pcb, E_READY);
                	sem_post(&sem_READY);
				}
				destruir_buffer(buffer);
				printf("\n FIN SIGNAL \n");
				break;
			case OUT_OF_MEMORY:
				buffer = recibir_buffer_completo(fd_cpu_dispatch);
				pcb = deserializar_pcb(buffer);
				destruir_buffer(buffer);
				
				enviar_a_exit(pcb, "OUT_OF_MEMORY");
				pcb_destruir(pcb);

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

void bloquear_proceso(t_pcb *pcb, char* motivo){	
	printf("\n\n DENTRO DE BLOQUEAR_PROCESO ANTES CAMBIO_DE_ESTADO\n\n");
	cambio_de_estado(pcb, E_BLOCKED);
	//pcb->estado = E_BLOCKED;
	printf("\n\n DENTRO DE BLOQUEAR_PROCESO ANTES DE AGREGAR A BLOCKED\n\n");
	pthread_mutex_lock(&mutex_BLOCKED);
	list_add(BLOCKED, pcb);
	pthread_mutex_unlock(&mutex_BLOCKED);
	log_info(logger_kernel, "PID: %i - Motivo de bloqueo: %s", pcb->pid, motivo);
}

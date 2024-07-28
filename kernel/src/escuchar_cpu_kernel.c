#include "escuchar_cpu_kernel.h"

// pthread_mutex_t mutex_READY;
pthread_mutex_t mutex_BLOCKED;

void escuchar_mensajes_dispatch_kernel()
{
	bool desconexion_dispatch_kernel = 0;
	while (!desconexion_dispatch_kernel)
	{
		
		int cod_op = recibir_operacion(fd_cpu_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch (cod_op)
		{
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

			
		case FIN_DE_QUANTUM:

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
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?
			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			tiempo = extraer_int_del_buffer(buffer);
   			log_info(logger_kernel, " Se recibio solicitud de sleep para la interfaz: %s por el tiempo:%d",nombre_interfaz,tiempo);
		
			indice_interfaz = buscar_interfaz(nombre_interfaz);
			if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
				break;
			}

			interfaz = list_get(interfaces, indice_interfaz);

			bloquear_proceso(pcb, interfaz->nombre);
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

			//destruir_buffer(buffer1);
			printf("destruir_buffer \n");
			break;

		case IO_STDIN_READ_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?

			nombre_interfaz = extraer_string_del_buffer(buffer);
			registro_direccion = extraer_int_del_buffer(buffer);
			registro_tamanio = extraer_int_del_buffer(buffer);

			indice_interfaz = buscar_interfaz(nombre_interfaz);
			if(!verificar_existencia_de_interfaz(indice_interfaz, pcb)){
				break;
			}
			interfaz = list_get(interfaces, indice_interfaz);

			destruir_buffer(buffer);

			buffer = crear_buffer();
			paquete = crear_paquete(IO_STDIN_READ_FS, buffer);
			cargar_int_al_buffer(paquete->buffer,pcb->pid);
			cargar_int_al_buffer(paquete->buffer, registro_direccion);
			cargar_int_al_buffer(paquete->buffer, registro_tamanio);

			pthread_mutex_lock(&interfaz->mutex_interfaz);
			list_add(interfaz->cola_espera, paquete);
			pthread_mutex_unlock(&interfaz->mutex_interfaz);

			sem_post(&interfaz->sem_espera);

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);

			//destruir_buffer(buffer);

			break;

		case IO_STDOUT_WRITE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?

			nombre_interfaz = extraer_string_del_buffer(buffer);
			registro_direccion = extraer_int_del_buffer(buffer);
			registro_tamanio = extraer_int_del_buffer(buffer);

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

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);

			//destruir_buffer(buffer);

			break;

		case IO_FS_READ_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			dir_logica = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			puntero_archivo = extraer_int_del_buffer(buffer);

			
			//resolver io_fs_read
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
			cargar_string_al_buffer(paquete->buffer, dir_logica);
			cargar_int_al_buffer(paquete->buffer, tamanio);
			cargar_int_al_buffer(paquete->buffer, puntero_archivo);
			
			pthread_mutex_lock(&interfaz->mutex_interfaz);
			list_add(interfaz->cola_espera, paquete);
			pthread_mutex_unlock(&interfaz->mutex_interfaz);

			sem_post(&interfaz->sem_espera);

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);

			//destruir_buffer(buffer);

			break;
		case IO_FS_WRITE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			dir_logica = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			puntero_archivo = extraer_int_del_buffer(buffer);

			//resolver io_fs_write
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
			cargar_string_al_buffer(paquete->buffer, dir_logica);
			cargar_int_al_buffer(paquete->buffer, tamanio);
			cargar_int_al_buffer(paquete->buffer, puntero_archivo);
			
			pthread_mutex_lock(&interfaz->mutex_interfaz);
			list_add(interfaz->cola_espera, paquete);
			pthread_mutex_unlock(&interfaz->mutex_interfaz);

			sem_post(&interfaz->sem_espera);

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);

			//destruir_buffer(buffer);

			break;

		case IO_FS_TRUNCATE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			
			//resolver io_fs_truncate
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

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);

			
			//destruir_buffer(buffer);

			break;
		case IO_FS_CREATE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			//resolver io_fs_create
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

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);
			
			//destruir_buffer(buffer);

			break;
		case IO_FS_DELETE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);

			//resolver io_fs_delete
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

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb, interfaz->nombre);
			sem_post(&sem_EXEC);
			
			//destruir_buffer(buffer);

			break;

		

		// case ELIMINAR_PROCESO:
		case KERNEL_EXIT:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);
			sem_post(&sem_desalojo);
			sem_post(&sem_EXEC);
			
			enviar_a_exit(pcb, "SUCCESS");
			destruir_buffer(buffer);
			// CONTEMPLAR EL CASO PARA DEVOLVER RECURSOS SI LOS TIENE
			break;

		case KERNEL_WAIT: // ponernos de acuerdo con nacho como envia el recurso solicitado
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);

			sem_post(&sem_desalojo);
			usleep(20);
			pcb->quantum = RUNNING->quantum;

			// RECURSOS = ["RA", "RB", "RC"];
			char *recurso_solicitado = extraer_string_del_buffer(buffer); // "RB"

			if (buscar_recurso(recurso_solicitado) < 0)
			{
				enviar_a_exit(pcb, "INVALID_RESOURCE");
			}
			else
			{
				restar_instancia(recursos_disponibles, pcb);
			}

			sem_post(&sem_EXEC);

			destruir_buffer(buffer);
			break;

		case KERNEL_SIGNAL:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);
			char *recurso = extraer_string_del_buffer(buffer);

			sem_post(&sem_desalojo);
			pcb->quantum = RUNNING->quantum;

			sumar_instancia(recurso, pcb);

			destruir_buffer(buffer);

			sem_post(&sem_EXEC);
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

void bloquear_proceso(t_pcb *pcb, char* motivo)
{
	cambio_de_estado(pcb, E_BLOCKED);
	//pcb->estado = E_BLOCKED;
	pthread_mutex_lock(&mutex_BLOCKED);
	list_add(BLOCKED, pcb);
	pthread_mutex_unlock(&mutex_BLOCKED);
	log_info("PID: %i -Motivo de bloque: %s", pcb->pid, motivo);
	
}


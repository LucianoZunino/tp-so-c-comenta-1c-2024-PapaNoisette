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
			t_paquete *paquete;

			
		case FIN_DE_QUANTUM:

			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer); // chequear si anda, sino usar deserealizar_pcb
			sem_post(&sem_EXEC);

			pcb->quantum = quantum;

			pthread_mutex_lock(&mutex_READY);
			list_add(READY, pcb);
			pthread_mutex_unlock(&mutex_READY);
			cambio_de_estado(pcb, E_READY);
			//pcb->estado = E_READY;
			break;

		case IO_GEN_SLEEP_FS:
			printf("flag1 \n");
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			printf("flag2 \n");
			pcb = deserializar_pcb(buffer);	 // chequear si anda, sino usar deserealizar_pcb
			printf("flag3 \n");
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?
			printf("flag4 \n");
			sem_post(&sem_desalojo);
			printf("flag5 \n");
			bloquear_proceso(pcb);
			printf("flag6 \n");

			nombre_interfaz = extraer_string_del_buffer(buffer);
			printf("flag7 \n");
			tiempo = extraer_int_del_buffer(buffer);
			printf("flag8 \n");
			indice_interfaz = buscar_interfaz(nombre_interfaz);
			printf("flag9 \n");
			interfaz = list_get(interfaces, indice_interfaz);
			printf("flag10 \n");

			destruir_buffer(buffer);
			printf("flag11 \n");
			buffer = crear_buffer();
			printf("flag12 \n");
			paquete = crear_paquete(IO_GEN_SLEEP_FS, buffer); // aca no iria generica?
			printf("flag13 \n");
			cargar_int_al_buffer(paquete->buffer,pcb->pid);
			printf("flag14 \n");
			cargar_int_al_buffer(paquete->buffer, tiempo);
			printf("flag15 \n");

			pthread_mutex_lock(&interfaz->mutex_interfaz);
			printf("flag16 \n");
			list_add(interfaz->cola_espera, paquete);
			printf("flag17 \n");
			pthread_mutex_unlock(&interfaz->mutex_interfaz);
			printf("flag18 \n");

			sem_post(&interfaz->sem_espera);
			printf("flag19 \n");

			destruir_buffer(buffer);

			break;

		case IO_STDIN_READ_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?

			nombre_interfaz = extraer_string_del_buffer(buffer);
			registro_direccion = extraer_int_del_buffer(buffer);
			registro_tamanio = extraer_int_del_buffer(buffer);

			indice_interfaz = buscar_interfaz(nombre_interfaz);
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
			bloquear_proceso(pcb);

			destruir_buffer(buffer);

			break;

		case IO_STDOUT_WRITE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	
			pcb->quantum = RUNNING->quantum; // Es necesario esperar al planificador?

			nombre_interfaz = extraer_string_del_buffer(buffer);
			registro_direccion = extraer_int_del_buffer(buffer);
			registro_tamanio = extraer_int_del_buffer(buffer);

			indice_interfaz = buscar_interfaz(nombre_interfaz);
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
			bloquear_proceso(pcb);

			destruir_buffer(buffer);

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
			bloquear_proceso(pcb);

			destruir_buffer(buffer);

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
			bloquear_proceso(pcb);

			destruir_buffer(buffer);

			break;

		case IO_FS_TRUNCATE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			
			//resolver io_fs_truncate
			indice_interfaz = buscar_interfaz(nombre_interfaz);
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
			bloquear_proceso(pcb);

			
			destruir_buffer(buffer);

			break;
		case IO_FS_CREATE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);
			//resolver io_fs_create
			indice_interfaz = buscar_interfaz(nombre_interfaz);
			interfaz = list_get(interfaces, indice_interfaz);
			
			paquete = crear_paquete(IO_FS_CREATE_FS, buffer);
			cargar_int_al_buffer(paquete->buffer, pcb->pid);
			cargar_string_al_buffer(paquete->buffer, nombre_archivo);

			pthread_mutex_lock(&interfaz->mutex_interfaz);
			list_add(interfaz->cola_espera, paquete);
			pthread_mutex_unlock(&interfaz->mutex_interfaz);

			sem_post(&interfaz->sem_espera);

			sem_post(&sem_desalojo);
			bloquear_proceso(pcb);
			
			destruir_buffer(buffer);

			break;
		case IO_FS_DELETE_FS:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);	 
			nombre_interfaz = extraer_string_del_buffer(buffer);
			nombre_archivo = extraer_string_del_buffer(buffer);

			//resolver io_fs_delete
			indice_interfaz = buscar_interfaz(nombre_interfaz);
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
			bloquear_proceso(pcb);
			
			destruir_buffer(buffer);

			break;

		

		// case ELIMINAR_PROCESO:
		case KERNEL_EXIT:
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);
			sem_post(&sem_desalojo);
			sem_post(&sem_EXEC);
			
			enviar_a_exit(pcb);
			destruir_buffer(buffer);
			// CONTEMPLAR EL CASO PARA DEVOLVER RECURSOS SI LOS TIENE
			break;

		case KERNEL_WAIT: // pónernos de acuerdo con nacho como envia el recurso solicitado
			buffer = recibir_buffer_completo(fd_cpu_dispatch);
			pcb = deserializar_pcb(buffer);

			sem_post(&sem_desalojo);
			pcb->quantum = RUNNING->quantum;

			// RECURSOS = ["RA", "RB", "RC"];
			char *recurso_solicitado = extraer_string_del_buffer(buffer); // "RB"

			if (buscar_recurso(recurso_solicitado) < 0)
			{
				enviar_a_exit(RUNNING); // no deberia ser el pcb DESEREALIZADO? 
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
			// aca no se que mas hacen..
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

void bloquear_proceso(t_pcb *pcb)
{
	cambio_de_estado(pcb, E_BLOCKED);
	//pcb->estado = E_BLOCKED;
	pthread_mutex_lock(&mutex_BLOCKED);
	list_add(BLOCKED, pcb);
	pthread_mutex_unlock(&mutex_BLOCKED);

	sem_post(&sem_EXEC);
}


// creo que este hilo vuela, interrput es unidireccional de kernel a memoria
/*void escuchar_mensajes_interrupt_kernel(){
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
*/
/*
1) ENTRADA/SALIDA (hecho con una sola io)
2) CONSOLA
3) HACER RECURSOS (hecho)
4) QUANTUM DE RR Y VRR (hecho)
5) HACER VRR E INTERRUPCION DE QUANTUM (hecho)
6) ELIMINAR PROCESO (TERMINAR LARGO PLAZO) (hecho)


*/


#include "escuchar_memoria_entradasalida.h"

char* datos;

void escuchar_mensajes_memoria_entradasalida(){
	printf("\nHola estoy escuchando memoria desde el socket: %i\n", fd_memoria);
	
    bool desconexion_memoria_entradasalida = 0;
	while(!desconexion_memoria_entradasalida){
		int cod_op = recibir_operacion(fd_memoria); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer;
			int pid;

			case IO_STDOUT_WRITE_FS:
				buffer = recibir_buffer_completo(fd_memoria);

				pid = extraer_int_del_buffer(buffer);
				char* mensaje = extraer_string_del_buffer(buffer);
				printf("\n%s\n\n", mensaje);

				notificar_fin(fd_kernel, pid);
				destruir_buffer(buffer);
				sem_post(&sem_stdout);
				break;
			case IO_FS_WRITE_FS:
				
				buffer = recibir_buffer_completo(fd_memoria);

				pid = extraer_int_del_buffer(buffer);
				datos = extraer_string_del_buffer(buffer);

				printf("Datos recibidos de memoria: %s", datos);

				destruir_buffer(buffer);

				printf("\nANTES DEL SEMAFORO\n");
				sem_post(&sem_fs_write);
				printf("\n despues del semaforo \n\n");
				break;
			case MEMORIA_ERROR:
				
				buffer = recibir_buffer_completo(fd_memoria);
				pid = extraer_int_del_buffer(buffer);
				int dir_fisica = extraer_int_del_buffer(buffer);
				log_error(logger_entradasalida, "Error, no se pudo leer en el PID: %i direccion fisica: %i\n ", pid, dir_fisica);

				sem_post(&sem_stdout);
      			
      			destruir_buffer(buffer);
				break;
			case -1:
				log_error(logger_entradasalida, "La Memoria se desconecto de Entradasalida. Terminando servidor.");
				desconexion_memoria_entradasalida = 1;
				break;
			default:
				log_warning(logger_entradasalida, "Operacion desconocida de Memoria-Entradasalida.");
				break;
			}
	}
}
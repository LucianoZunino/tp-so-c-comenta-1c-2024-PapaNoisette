#include "escuchar_kernel_entradasalida.h"
#include <readline/readline.h>



void escuchar_instrucciones_generica(){
	printf("escuchar1\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case IO_GEN_SLEEP_FS:
				t_buffer* buffer;
				buffer = recibir_buffer_completo(fd_kernel);
				
				int pid = extraer_int_del_buffer(buffer);
				int unidades = extraer_int_del_buffer(buffer);

				usleep(unidades * tiempo_unidad_trabajo);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				t_buffer* buffer_kernel;
				buffer_kernel = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer_kernel);
				free(buffer_kernel);

				buffer_kernel = crear_buffer();
				cargar_int_al_buffer(buffer_kernel, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer_kernel);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}


void escuchar_instrucciones_stdin(){
	printf("escuchar2\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer;
			int pid;
			case IO_STDIN_READ_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				printf("\nIngrese hasta %i caracteres\n", reg_tamanio);
				char* input = readline("> ");
				char* mensaje;
				memcpy(mensaje, input, reg_tamanio);

				solicitar_almacen_memoria(reg_direccion, mensaje);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_stdout(){
	printf("escuchar3\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer = crear_buffer();
			int pid;
			case IO_STDOUT_WRITE_FS:
				printf("a\n");
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				solicitar_lectura_memoria(reg_direccion, reg_tamanio);

				sem_wait(&sem_stdout);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
				
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;

			default: // La instruccion es incorrecta
				printf("b\n");
				buffer = recibir_buffer_completo(fd_kernel);
				printf("c\n");
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				//buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_dialfs(){
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer = crear_buffer();
			int pid;
			char* nombre;
			t_config* config;
			case IO_FS_CREATE_FS:

				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

				// Chequear si hay lugar en bitmap
				int direccion = buscar_lugar_bitmap(1);
				// Asignar lugar en bitmap
				bitarray_set_bit(bitmap, direccion);
				// Crear archivo metadata
				FILE* archivo = fopen(nombre, "w");
				fclose(archivo);

				config = config_create(nombre);

				config_set_value(config, "BLOQUE_INICIAL", string_itoa(direccion));
				config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(0));
				config_save(config);
				// Sincronizar
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				config_destroy(config);
				break;

			case IO_FS_DELETE_FS:
				break;

			case IO_FS_WRITE_FS:
				break;
			
			case IO_FS_READ_FS:
				break;

			case IO_FS_TRUNCATE_FS:
				break;
				
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;

			default: // La instruccion es incorrecta
				printf("b\n");
				buffer = recibir_buffer_completo(fd_kernel);
				printf("c\n");
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				//buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}



void notificar_fin(int fd_kernel, int pid){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, pid);

	t_paquete* paquete = crear_paquete(FIN_IO, buffer);
	enviar_paquete(paquete, fd_kernel);

	eliminar_paquete(paquete);
	free(buffer);
}

void solicitar_lectura_memoria(int direccion, int tamanio){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_int_al_buffer(buffer, tamanio);

	t_paquete* paquete = crear_paquete(IO_STDOUT_WRITE_FS, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	free(buffer);
}

void solicitar_almacen_memoria(int direccion, char* mensaje){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_string_al_buffer(buffer, mensaje);

	t_paquete* paquete = crear_paquete(IO_STDIN_READ_FS, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	free(buffer);
}

int buscar_lugar_bitmap(int tamanio){
	int i = 0;
	int contador_libres = 0;
	int contador_libres_continuo = 0;
	while(i < block_count){
		if(bitarray_test_bit(bitmap, i) == 0){
			contador_libres ++;
			contador_libres_continuo ++;
			if (contador_libres_continuo == tamanio){return i - tamanio;}
		}
		if(bitarray_test_bit(bitmap, i) == 1){
			contador_libres_continuo = 0;
		}

		i ++;
	}

	if(contador_libres >= tamanio){
		// TODO: comprimir() -> buscar
	}
	else{
		// TODO: DEVUELVE ERROR, NO HAY ESPACIO.
		return -1;
	}
}

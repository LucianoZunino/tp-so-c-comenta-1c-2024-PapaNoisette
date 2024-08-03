#include "escuchar_entradasalida_memoria.h"
//int socket_entradasalida;

void escuchar_mensajes_entradasalida_memoria(int socket_entradasalida){
	t_buffer* buffer;
	int dir_fisica;
	int tamanio;
	int pid;
	// t_interfaz* interfaz = list_get(lista_de_interfaces, indice);

	// socket_entradasalida = *interfaz->socket;
	// printf("\nSOCKET ENTRADASALIDA: %i\n", socket_entradasalida);

    bool desconexion_entradasalida_memoria = 0;
	
	while(!desconexion_entradasalida_memoria){
		int cod_op = recibir_operacion(socket_entradasalida); // recv() es bloqueante por ende no queda loopeando infinitamente

		switch(cod_op){
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_memoria, socket_entradasalida, cod_op);
				break;
			case IO_STDIN_READ_FS:
				log_info(logger_memoria, "IO_STDIN_READ_FS");
				
				buffer = recibir_buffer_completo(socket_entradasalida);
				pid = extraer_int_del_buffer(buffer);
				dir_fisica = extraer_int_del_buffer(buffer);

				//tamanio = extraer_int_del_buffer(buffer); //no se si no hace falta el tamaño
				char* datos = extraer_string_del_buffer(buffer);

				printf("\nMensaje recibido de entrada y salida: %s\n", datos);
				
				ejecutar_stdin_read(socket_entradasalida, pid, strlen(datos), dir_fisica, datos);
				
				destruir_buffer(buffer);
				break;
			case IO_STDOUT_WRITE_FS:
				log_info(logger_memoria, "IO_STDOUT_WRITE_FS");
				buffer = recibir_buffer_completo(socket_entradasalida);
				pid = extraer_int_del_buffer(buffer);
				dir_fisica = extraer_int_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);
				ejecutar_stdout_write(socket_entradasalida, pid, tamanio, dir_fisica, IO_STDOUT_WRITE_FS);
				destruir_buffer(buffer);
				break;
			case IO_FS_WRITE_FS:
				log_info(logger_memoria, "IO_FS_WRITE_FS");
				buffer = recibir_buffer_completo(socket_entradasalida);
				pid = extraer_int_del_buffer(buffer);
				dir_fisica = extraer_int_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);
				ejecutar_stdout_write(socket_entradasalida, pid, tamanio, dir_fisica, IO_FS_WRITE_FS);
				destruir_buffer(buffer);
				break;
			case -1:
				log_error(logger_memoria, "El modulo de Entradasalida se desconecto de Memoria. Terminando servidor.");
				desconexion_entradasalida_memoria = 1;
				break;
			default:
				log_warning(logger_memoria, "Operacion desconocida de Entradasalida-Memoria.");
				break;
		}
	}
}

void ejecutar_stdin_read(int socket_entradasalida, int pid, int tamanio , int dir_fisica, char* datos){

	void* datos_aux = (void*)datos; //posible malloc

	ejecutar_mov_out(tamanio, dir_fisica, pid, datos_aux);
}

void ejecutar_stdout_write(int socket_entradasalida, int pid, int tamanio, int dir_fisica, int cod_op){

	void* datos_aux = ejecutar_mov_in(tamanio, dir_fisica, pid);
	char* datos_a_devolver = malloc(tamanio);
	t_buffer* buffer = crear_buffer();
	t_paquete* paquete;

	if(datos_aux == NULL){
		log_error(logger_memoria, "El proceso no tiene suficientes paginas asignadas para leer %i bytes \n", tamanio);
		cargar_int_al_buffer(buffer, pid);
		cargar_int_al_buffer(buffer, dir_fisica);
		paquete = crear_paquete(MEMORIA_ERROR, buffer);
	}else{
		memcpy(datos_a_devolver, datos_aux, tamanio);
		
		printf("\n\nChar* datos a devolver: %s \n\n", datos_a_devolver);

		// ES UN PRINT PARA VER LOS DATOS A ENVIAR EN BYTES POR EL VOID*
		if(datos_a_devolver != NULL){
			printf("##### DATOS A DEVOLVER DEL MOV_IN: #####\n");
			unsigned char* byte_datos = (unsigned char*)datos_a_devolver;
			for(int i = 0; i < tamanio; i++){
				printf("byte %d: %02X\n", i, byte_datos[i]);
			}
			printf("\n");
		}
		// --------------------------------------------------------------

		cargar_int_al_buffer(buffer, pid);
		cargar_string_al_buffer(buffer, datos_a_devolver);
		paquete = crear_paquete(cod_op, buffer);
	}
	
	enviar_paquete(paquete, socket_entradasalida); // socket es interfaz->socket
	eliminar_paquete(paquete);
	free(datos_a_devolver);
}

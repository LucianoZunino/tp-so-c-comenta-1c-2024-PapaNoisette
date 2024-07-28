#include "escuchar_entradasalida_memoria.h"


void escuchar_mensajes_entradasalida_memoria(int indice){
		t_buffer* buffer;
		int dir_fisica;
		int tamanio;
		int pid;
		t_interfaz* interfaz = list_get(lista_de_interfaces, indice);

    bool desconexion_entradasalida_memoria = 0;
	while(!desconexion_entradasalida_memoria){
		int cod_op = recibir_operacion(interfaz->socket); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_memoria, fd_entradasalida, cod_op);
				break;
			case IO_STDIN_READ_FS:

				log_info(logger_memoria, "IO_STDIN_READ_FS");
				buffer = recibir_buffer_completo(fd_entradasalida);
				//no se si no hace falta el tamaño
				pid = extraer_int_del_buffer(buffer);
				dir_fisica = extraer_int_del_buffer(buffer);
				tamanio = extraer_int_del_buffer(buffer);
				char* datos = extraer_string_del_buffer(buffer);
				ejecutar_stdin_read(pid, tamanio, dir_fisica, datos);
				destruir_buffer(buffer);
				break;
			
			case IO_STDOUT_WRITE_FS:
				log_info(logger_memoria, "IO_STDOUT_WRITE_FS");
				buffer = recibir_buffer_completo(fd_entradasalida);
				pid = extraer_int_del_buffer(buffer);
				dir_fisica = extraer_int_del_buffer(buffer);
				tamanio=extraer_int_del_buffer(buffer);
				ejecutar_stdout_write(pid, tamanio ,dir_fisica);
				destruir_buffer(buffer);
				break;

			case -1:
				log_error(logger_memoria, "La Entradasalida se desconecto de Memoria. Terminando servidor.");
				desconexion_entradasalida_memoria = 1;
				break;
			default:
				log_warning(logger_memoria, "Operacion desconocida de Entradasalida-Memoria.");
				break;
			}
	}
}

void ejecutar_stdin_read(int pid, int tamanio , int dir_fisica, char* datos){
	void* datos_aux = (void*) datos; //posible malloc
	ejecutar_mov_out(tamanio, dir_fisica, pid, datos_aux);
}

void ejecutar_stdout_write(int pid, int tamanio, int dir_fisica){
	void* datos_a_devolver = ejecutar_mov_in(tamanio, dir_fisica, pid);

	t_buffer* buffer = crear_buffer();

	if(datos_a_devolver == NULL){
		log_error(logger_memoria, "El proceso no tiene suficientes paginas asignadas para leer %i bytes \n", tamanio);
		cargar_int_al_buffer(buffer, pid);
		cargar_int_al_buffer(buffer, dir_fisica);
		t_paquete* paquete = crear_paquete(MEMORIA_ERROR, buffer);
	}else{
		cargar_datos_al_buffer(buffer, datos_a_devolver, tamanio);
		t_paquete* paquete = crear_paquete(MEMORIA_MOV_IN, buffer);
	}
}

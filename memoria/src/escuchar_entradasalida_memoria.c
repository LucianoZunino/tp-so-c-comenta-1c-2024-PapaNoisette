#include "escuchar_entradasalida_memoria.h"

void escuchar_mensajes_entradasalida_memoria(){
		t_buffer* buffer;
		int dir_fisica;
		int tamanio;

    bool desconexion_entradasalida_memoria = 0;
	while(!desconexion_entradasalida_memoria){
		int cod_op = recibir_operacion(fd_entradasalida); // recv() es bloqueante por ende no queda loopeando infinitamente
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
			dir_fisica = extraer_int_del_buffer(buffer);
			tamanio=extraer_int_del_buffer(buffer);
			void * datos = extraer_datos_del_buffer(buffer);
			ejecutar_stdin_read(dir_fisica,datos);
			destruir_buffer(buffer);
			break;
			
			case IO_STDOUT_WRITE_FS:
			log_info(logger_memoria, "IO_STDOUT_WRITE_FS");
			buffer = recibir_buffer_completo(fd_entradasalida);
			dir_fisica = extraer_int_del_buffer(buffer);
			tamanio=extraer_int_del_buffer(buffer);
			ejecutar_stdout_write(dir_fisica);
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

void ejecutar_stdin_read(int dir_fisica,void * datos){
//aca guardo en memoria lo que se ingreso en el modulo i/o por teclado
}

void ejecutar_stdout_write(int dir_fisica,int tamanio){
//aca le devuelvo a io la info que me solicita, io lo va a mostrar por pantalla
}

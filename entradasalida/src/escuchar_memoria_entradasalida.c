#include "escuchar_memoria_entradasalida.h"

void escuchar_mensajes_memoria_entradasalida(){
    bool desconexion_memoria_entradasalida = 0;
	while(!desconexion_memoria_entradasalida){
		int cod_op = recibir_operacion(fd_memoria); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case IO_STDOUT_WRITE_FS:
				t_buffer* buffer;
				buffer = recibir_buffer_completo(fd_memoria);

				char* mensaje = extraer_string_del_buffer(buffer);
				printf(mensaje);

				destruir_buffer(buffer);
				sem_post(&sem_stdout);
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
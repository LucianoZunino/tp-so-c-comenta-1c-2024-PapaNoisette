#include "escuchar_entradasalida_memoria.h"

void escuchar_mensajes_entradasalida_memoria(){
    bool desconexion_entradasalida_memoria = 0;
	while(!desconexion_entradasalida_memoria){
		int cod_op = recibir_operacion(fd_entradasalida); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_memoria, fd_entradasalida, cod_op);
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
#include "escuchar_entradasalida_kernel.h"

void escuchar_mensajes_entradasalida_kernel(){
    bool desconexion_entradasalida_kernel = 0;
	while(!desconexion_entradasalida_kernel){
		int cod_op = recibir_operacion(fd_entradasalida); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case HANDSHAKE_ENTRADASALIDA:
				aceptar_handshake(logger_kernel, fd_entradasalida, cod_op);
				break;
			case -1:
				log_error(logger_kernel, "La Entradasalida se desconecto de Kernel. Terminando servidor.");
				desconexion_entradasalida_kernel = 1;
				break;
			default:
				log_warning(logger_kernel, "Operacion desconocida de Entradasalida-Kernel.");
				break;
			}
	}
}
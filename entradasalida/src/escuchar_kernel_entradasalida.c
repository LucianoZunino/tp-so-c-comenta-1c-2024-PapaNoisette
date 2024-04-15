#include "escuchar_kernel_entradasalida.h"

void escuchar_mensajes_kernel_entradasalida(){
    bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de Entradasalida. Terminando servidor.");
				desconexion_kernel_entradasalida = 1;
				break;
			default:
				log_warning(logger_entradasalida, "Operacion desconocida de Kernel-Entradasalida.");
				break;
			}
	}
}
#include "escuchar_kernel_memoria.h"

void escuchar_mensajes_kernel_memoria(){
    bool desconexion_kernel_memoria = 0;
	while(!desconexion_kernel_memoria){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_memoria, "El Kernel se desconecto de Memoria. Terminando servidor.");
				desconexion_kernel_memoria = 1;
				break;
			default:
				log_warning(logger_memoria, "Operacion desconocida de Kernel-Memoria.");
				break;
			}
	}
}
#include "escuchar_kernel_cpu.h"

void escuchar_mensajes_kernel_dispatch(){
    bool desconexion_kernel_dispatch = 0;
	while(!desconexion_kernel_dispatch){
		int cod_op = recibir_operacion(fd_kernel_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
						
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_cpu, fd_kernel_dispatch, cod_op);
				break;
			case -1:
				log_error(logger_cpu, "El Kernel se desconecto de Dispatch. Terminando servidor.");
				desconexion_kernel_dispatch = 1;
				break;
			default:
				log_warning(logger_cpu, "Operacion desconocida de Kernel-Dispatch.");
				break;
			}
	}
}

void escuchar_mensajes_kernel_interrupt(){
    bool desconexion_kernel_interrupt = 0;
	while(!desconexion_kernel_interrupt){
		int cod_op = recibir_operacion(fd_kernel_interrupt); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_cpu, "El Kernel se desconecto de Interrupt. Terminando servidor.");
				desconexion_kernel_interrupt = 1;
				break;
			default:
				log_warning(logger_cpu, "Operacion desconocida de Kernel-Interrupt.");
				break;
			}
	}
}
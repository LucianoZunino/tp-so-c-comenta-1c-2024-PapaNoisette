#include "escuchar_cpu_memoria.h"

void escuchar_mensajes_cpu_memoria(){
    bool desconexion_cpu_memoria = 0;
	while(!desconexion_cpu_memoria){
		int cod_op = recibir_operacion(fd_cpu); // O ES CPU_DISPATCH ? // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_memoria, "La CPU se desconecto de Memoria. Terminando servidor.");
				desconexion_cpu_memoria = 1;
				break;
			default:
				log_warning(logger_memoria, "Operacion desconocida de CPU-Memoria.");
				break;
			}
	}
}
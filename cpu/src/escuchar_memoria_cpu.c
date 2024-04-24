#include "escuchar_memoria_cpu.h"

void escuchar_mensajes_memoria_cpu(){
    bool desconexion_memoria_cpu= 0;
	while(!desconexion_memoria_cpu){
		int cod_op = recibir_operacion(fd_memoria); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case -1:
				log_error(logger_cpu, "La Memoria se desconecto de CPU. Terminando servidor.");
				desconexion_memoria_cpu = 1;
				break;
			default:
				log_warning(logger_cpu, "Operacion desconocida de Memoria-CPU.");
				break;
			}
	}
}
#include "escuchar_cpu_memoria.h"

void escuchar_mensajes_cpu_memoria(){
    bool desconexion_cpu_memoria = 0;
	while(!desconexion_cpu_memoria){
		int cod_op = recibir_operacion(fd_cpu); // O ES CPU_DISPATCH ? // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case MEMORIA_SOLICITAR_INSTRUCCION:
				t_buffer* buffer = recibir_buffer_completo(fd_cpu);
				t_pcb* pcb = deserializar_pcb(buffer);
				usleep(retardo_respuesta);
				enviar_instruccion_a_cpu(pcb, fd_cpu);

			case HANDSHAKE_CPU:
				aceptar_handshake(logger_memoria, fd_cpu, cod_op);
				break;
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

void enviar_instruccion_a_cpu(t_pcb* pcb, int socket){

	for (int i = 0; i <= list_size(lista_de_miniPcb); i++){
		
		t_miniPcb* miniPcb = list_get(lista_de_miniPcb, i);
		
		if (miniPcb->pid == pcb->pid){
			
			int pc = pcb->program_counter;
			char* instruccion = list_get(miniPcb->lista_de_instrucciones, pc);
			enviar_instruccion(instruccion, socket);
			return;
		}


	}
	log_info(logger_memoria,"Error, no hay pid cargado en memoria \n");
}


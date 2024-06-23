#include "escuchar_cpu_memoria.h"

void escuchar_mensajes_cpu_memoria()
{
	bool desconexion_cpu_memoria = 0;
	t_buffer* buffer;
	 while(!desconexion_cpu_memoria)
	//while (1)
	{
		int pid;
		int pagina;
		int marco;
		int tamanio;
		int dir_fisica;
		int cod_op = recibir_operacion(fd_cpu);
		t_paquete *paquete;
		t_buffer *buffer_a_enviar ;
	    t_buffer *buffer ;

		printf("cod_op :%d\n", cod_op);

		switch (cod_op)
		{

		case HANDSHAKE_CPU:
			aceptar_handshake(logger_memoria, fd_cpu, cod_op);

			break;
		case CPU_SOLICITA_INSTRUCCION:
			buffer = recibir_buffer_completo(fd_cpu);
			t_pcb *pcb = deserializar_pcb(buffer);
			usleep(retardo_respuesta);
			enviar_instruccion_a_cpu(pcb, fd_cpu);
			destruir_buffer(buffer);

			break;
		case CPU_CONSULTA_TAM_PAGINA:
							log_info(logger_memoria, "CPU_CONSULTA_TAM_PAGINA");

			usleep(retardo_respuesta);
			buffer_a_enviar = crear_buffer();
			paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar);
			cargar_int_al_buffer(paquete->buffer, tam_pagina);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);
						destruir_buffer(buffer_a_enviar);

			break;
		case CPU_CONSULTA_FRAME: // no se si no hay que hacerlo tambieen en io
					log_info(logger_memoria, "CPU_CONSULTA_FRAME");

			usleep(retardo_respuesta);
			buffer = recibir_buffer_completo(fd_cpu);

			pid = recibir_int(fd_cpu);
			int pagina = extraer_int_del_buffer(buffer);
			int marco = -1;
			marco = buscar_marco(pid, pagina);
			buffer_a_enviar = crear_buffer();
			paquete = crear_paquete(CPU_CONSULTA_FRAME, buffer_a_enviar);
			cargar_int_al_buffer(paquete->buffer, marco);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);
			// LOG OBLIGATORIO
			log_info(logger_memoria, "Acceso a Tabla de Páginas PID: %d - Página: %d - Marco: %d\n", pid, pagina, marco);
			destruir_buffer(buffer);

			break;
		case MEMORIA_RESIZE:
		log_info(logger_memoria, "RESIZE");

			buffer = recibir_buffer_completo(fd_cpu);
			 pid = extraer_int_del_buffer(buffer);
			int nuevo_tamano = extraer_int_del_buffer(buffer);
			usleep(retardo_respuesta);

			TablaDePaginasPorProceso *tabla_de_paginas_del_proceso = buscar_tabla_por_pid(lista_de_tablas_de_paginas_por_proceso, pid);
			int resultado = resize_tamano_proceso(tabla_de_paginas_del_proceso, nuevo_tamano);
			if(resultado == 0){ // 0 -> Ok | -1 -> OUT_OF_MEMORY
				enviar_ok(RESIZE_OK, fd_cpu);
			}
			else{
				enviar_ok(OUT_OF_MEMORY, fd_cpu);

			}
			destruir_buffer(buffer);

			break;
		case MEMORIA_MOV_OUT:
		log_info(logger_memoria, "MOV_OUT");

		buffer = recibir_buffer_completo(fd_cpu);
			 pid = extraer_int_del_buffer(buffer);
			 dir_fisica = extraer_int_del_buffer(buffer);
			void * datos = extraer_datos_del_buffer(buffer);
			ejecutar_mov_out (pid,dir_fisica,datos);
			destruir_buffer(buffer);

		break;
		case MEMORIA_MOV_IN:
		log_info(logger_memoria, "MOV_IN");

		buffer = recibir_buffer_completo(fd_cpu);
			 pid = extraer_int_del_buffer(buffer);
			 dir_fisica = extraer_int_del_buffer(buffer);
			ejecutar_mov_in (pid,dir_fisica);
			destruir_buffer(buffer);

		case MEMORIA_COPY_STRING:
				log_info(logger_memoria, "COPY_STRING");

			buffer = recibir_buffer_completo(fd_cpu);
			pcb = deserializar_pcb(buffer);	 
		    tamanio = extraer_int_del_buffer(buffer);

			int dir_fisica_origen = extraer_int_del_buffer(buffer);
			int dir_fisica_estino = extraer_int_del_buffer(buffer);

			ejecutar_copy_string (pid,dir_fisica_origen,dir_fisica_estino,tamanio);
			destruir_buffer(buffer);

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

void ejecutar_copy_string (int pid,int dir_fisica_origen,int dir_fisica_estino,int tamanio){

}

void ejecutar_mov_out (int pid,int dir_fisica,void* datos){

}
void ejecutar_mov_in(int pid,int dir_fisica){

}

void enviar_instruccion_a_cpu(t_pcb *pcb, int socket)
{

	for (int i = 0; i <= list_size(lista_de_miniPcb); i++)
	{

		t_miniPcb *miniPcb = list_get(lista_de_miniPcb, i);

		if (miniPcb->pid == pcb->pid)
		{

			int pc = pcb->program_counter;
			char *instruccion = list_get(miniPcb->lista_de_instrucciones, pc);
			enviar_instruccion(instruccion, socket);
			return;
		}
	}
	log_info(logger_memoria, "Error, no hay pid cargado en memoria \n");
}

int buscar_marco(int pid, int num_pagina)
{
	int marco = -1;
	TablaDePaginasPorProceso *tabla_pag_proceso = buscar_tabla_por_pid(lista_de_tablas_de_paginas_por_proceso, pid);
	marco = tabla_pag_proceso->tabla_paginas->pagina[num_pagina].numero_de_marco; 
	return marco;
}

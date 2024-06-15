#include "escuchar_cpu_memoria.h"

void escuchar_mensajes_cpu_memoria()
{
	bool desconexion_cpu_memoria = 0;
	while (!desconexion_cpu_memoria)
	{
		int pid;
		int pagina;
		int marco;
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
			break;
		case CPU_CONSULTA_TAM_PAGINA:
			usleep(retardo_respuesta);
			buffer_a_enviar = crear_buffer();
			paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar);
			cargar_int_al_buffer(paquete->buffer, tam_pagina);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);
			break;
		case CPU_CONSULTA_FRAME: // no se si no hay que hacerlo tambieen en io
			usleep(retardo_respuesta);

			pid = recibir_int(fd_cpu);
			int pagina = recibir_int(fd_cpu);
			int marco = -1;
			marco = buscar_marco(pid, pagina);
			buffer_a_enviar = crear_buffer();
			paquete = crear_paquete(CPU_CONSULTA_FRAME, buffer_a_enviar);
			cargar_int_al_buffer(paquete->buffer, marco);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);
			// LOG OBLIGATORIO
			log_info(logger_memoria, "Acceso a Tabla de Páginas PID: %d - Página: %d - Marco: %d\n", pid, pagina, marco);

			break;
		case MEMORIA_RESIZE:
			usleep(retardo_respuesta);
			pid = recibir_int(fd_cpu);
			int nuevo_tam = recibir_int(fd_cpu);
			usleep(retardo_respuesta);

			TablaDePaginasPorProceso *tabla_de_paginas_del_proceso = buscar_tabla_por_pid(lista_de_tablas_de_paginas_por_proceso, pid);
			int resultado = resize_tamano_proceso(tabla_de_paginas_del_proceso, nuevo_tam);
			if (resultado == 0)
			{ // 0 -> Ok | -1 -> OUT_OF_MEMORY
				enviar_ok(RESIZE_OK, fd_cpu);
			}
			else
			{
				enviar_ok(OUT_OF_MEMORY, fd_cpu);
			}
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

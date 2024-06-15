#include "escuchar_cpu_memoria.h"

void escuchar_mensajes_cpu_memoria()
{
	bool desconexion_cpu_memoria = 0;
	// while(!desconexion_cpu_memoria){
	while (1)
	{
		sleep(2);
		int cod_op = recibir_operacion(fd_cpu);
		printf("cod_op :%d\n", cod_op);

		switch (cod_op)
		{

		case HANDSHAKE_CPU:
			aceptar_handshake(logger_memoria, fd_cpu, cod_op);
			break;
		case CPU_SOLICITA_INSTRUCCION:
			t_buffer *buffer = recibir_buffer_completo(fd_cpu);
			t_pcb *pcb = deserializar_pcb(buffer);
			usleep(retardo_respuesta);
			enviar_instruccion_a_cpu(pcb, fd_cpu);
			break;
		case CPU_CONSULTA_TAM_PAGINA:
			t_buffer *buffer_a_enviar = crear_buffer();
			t_paquete *paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar);
			cargar_int_al_buffer(paquete->buffer, tam_pagina);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);
			break;
		case CPU_CONSULTA_FRAME:
			break;
		case MEMORIA_RESIZE:

			t_buffer *buffer = recibir_buffer_completo(fd_cpu);
			int pid = recibir_int(fd_cpu);
			int nuevo_tam = recibir_int(fd_cpu);
			usleep(retardo_respuesta);
			resize_proceso(pid, nuevo_tam);
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

void resize_proceso(pid, nuevo_tam)
{
	int viejo_tam=0;
	int diferencia_tam;
	int cant_marcos_libres = 0;

	//calculo paginas libres en memoria
	for (int i = 0; i < cantidad_de_marcos; i++)
	{
		if (!frames[i].ocupado)
		{
			cant_marcos_libres++;
		}
	}
	//calculo tamaño viejo del proceso en memoria
	for (int i = 0; i < cantidad_de_marcos; i++)
	{
		if (!frames[i].ocupado &&frames[i].pid==pid)
		{
			viejo_tam+=tam_pagina;
		}
	}
	if (viejo_tam==nuevo_tam){
	log_info(logger_cpu, "Resize cancelado, no es ampliacion ni reduccion: %d", pid);
	exit;
	}

	int memoria_disponible = cant_marcos_libres * tam_pagina;
    
	//si es ampliacion , chequeo tener la memoria adicional que necesito
	//si es reduccion siempre el resize es ok
	if (memoria_disponible >= nuevo_tam-viejo_tam || nuevo_tam<=viejo_tam ) 
	{
		// resize OK
		t_buffer *buffer_a_enviar = crear_buffer();
		t_paquete *paquete = crear_paquete(RESIZE_OK, buffer_a_enviar);
		enviar_paquete(paquete, fd_cpu);
		eliminar_paquete(paquete);

			if (nuevo_tam>=viejo_tam ) 
			{//ampliacion
			// LOG OBLIGATORIO

			log_info(logger_cpu, "Ampliación de Proceso PID: %d -Tamaño Actual:%d - Tamaño a Ampliar: %d", pid, viejo_tam, nuevo_tam);


			}{//reduccion
				// LOG OBLIGATORIO

		log_info(logger_cpu, "Ampliación de Proceso PID: %d -Tamaño Actual:%d - Tamaño a reducir: %d", pid, viejo_tam, nuevo_tam);

			}

	}
	else // FAIL RESIZE
	{
		t_buffer *buffer_a_enviar = crear_buffer();
		t_paquete *paquete = crear_paquete(OUT_OF_MEMORY, buffer_a_enviar);
		enviar_paquete(paquete, fd_cpu);
		eliminar_paquete(paquete);
	}
	
}

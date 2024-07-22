#include "escuchar_cpu_memoria.h"
//void* memoria_RAM;
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
				t_paquete *paquete2;
		t_buffer *buffer_a_enviar2;

		t_buffer *buffer_a_enviar;
		t_buffer *buffer;
		int tamanio;
		int dir_fisica;
		switch (cod_op)
		{

		case HANDSHAKE_CPU:
			aceptar_handshake(logger_memoria, fd_cpu, cod_op);

			break;
		case CPU_SOLICITA_INSTRUCCION:
			printf("-CASE--CPU_SOLICITA_INSTRUCCION :\n");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			int program_counter = extraer_int_del_buffer(buffer);

			usleep(retardo_respuesta);
			enviar_instruccion_a_cpu(pid, program_counter, fd_cpu);
			destruir_buffer(buffer);
			break;
		case CPU_CONSULTA_TAM_PAGINA:
			printf("-CASE--CPU_CONSULTA_TAM_PAGINA :\n");

			buffer_a_enviar2 = crear_buffer();
			paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar2);
			cargar_int_al_buffer(paquete->buffer, tam_pagina);
			enviar_paquete(paquete, fd_cpu);
			eliminar_paquete(paquete);

			break;
		case CPU_CONSULTA_FRAME: // no se si no hay que hacerlo tambieen en io
			usleep(retardo_respuesta);
			/*
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
			*/
			break;
		case MEMORIA_RESIZE:

			log_info(logger_memoria, "MEMORIA_RESIZE");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			int nuevo_tamano = extraer_int_del_buffer(buffer);

			usleep(retardo_respuesta);

			// TablaDePaginasPorProceso *tabla_de_paginas_del_proceso = buscar_tabla_por_pid(lista_de_tablas_de_paginas_por_proceso, pid);
			int resultado = resize_tamano_proceso(pid, nuevo_tamano);
			if (resultado == 0)
			{ // 0 -> Ok | -1 -> OUT_OF_MEMORY

				log_info(logger_memoria, "RESIZE_OK");
				buffer_a_enviar = crear_buffer();
				paquete2 = crear_paquete(RESIZE_OK, buffer_a_enviar);
				enviar_paquete(paquete2, fd_cpu);
				eliminar_paquete(paquete2);

				// RESOLVER ESTA PARTE QUE QUEDA PEGAD EL BUFFER
			}
			else
			{
				log_info(logger_memoria, "RESIZE NOT OK OUT_OF_MEMORY");
				buffer_a_enviar = crear_buffer();
				paquete2 = crear_paquete(OUT_OF_MEMORY, buffer_a_enviar);
				enviar_paquete(paquete2, fd_cpu);
				eliminar_paquete(paquete2);
			}

			break;
		case MEMORIA_MOV_OUT: //LECTURA
			log_info(logger_memoria, "MOV_OUT");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			dir_fisica = extraer_int_del_buffer(buffer);
			printf("flag  mem_mov_out1\n");
			int datos = extraer_datos_del_buffer(buffer);
			printf("flag  mem_mov_out2\n");
			void* datos_aux = &datos;
			printf("flag  mem_mov_out3\n");
			printf("flag  mem_mov_out4\n");
			ejecutar_mov_out (tamanio, dir_fisica, datos_aux);
			usleep(retardo_respuesta);
			
			destruir_buffer(buffer);
			break;
		case MEMORIA_MOV_IN: //ESCRITURA
			log_info(logger_memoria, "MOV_IN");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			dir_fisica = extraer_int_del_buffer(buffer);
			printf("flag  mem_mov_in1\n");
			void* datos_a_devolver = ejecutar_mov_in(tamanio, dir_fisica);
			printf("flag  mem_mov_in2\n");
			buffer_a_enviar = crear_buffer();
			printf("flag  mem_mov_in3\n");
			//cargar_int_al_buffer(pid); CREO QUE NO ES NECESARIO ENVIARLO
			cargar_datos_al_buffer(buffer_a_enviar, datos_a_devolver, tamanio);
			paquete = crear_paquete(MEMORIA_MOV_IN, buffer_a_enviar); //SI COLISIONA CAMBIAR COD_OP
			enviar_paquete(paquete, fd_cpu);
			usleep(retardo_respuesta);
			eliminar_paquete(paquete);
			break;

		case MEMORIA_COPY_STRING: //DESDE CPU SE HACE DICHA LOGICA. SOLICITAS UNA LECTURA, CON EL DATO QUE TE DEVUELVE, REALIZAR UNA ESCRITURA.
			log_info(logger_memoria, "COPY_STRING");

			buffer = recibir_buffer_completo(fd_cpu);
			tamanio = extraer_int_del_buffer(buffer);

			int dir_fisica_origen = extraer_int_del_buffer(buffer);
			int dir_fisica_destino = extraer_int_del_buffer(buffer);

			void* aux = ejecutar_mov_in(tamanio, dir_fisica_origen);
			ejecutar_mov_out(tamanio, dir_fisica_destino, aux);


			destruir_buffer(buffer);
			break;

		case -1:
			log_error(logger_memoria, "La CPU se desconecto de Memoria. Terminando servidor.");
			desconexion_cpu_memoria = 1;
			break;

		default:
			log_warning(logger_memoria, "Operacion desconocida de CPU-Memoria. cod_op:%d", cod_op);
			break;
		}
	}
}

void ejecutar_copy_string(int pid, int dir_fisica_origen, int dir_fisica_destino, int tamanio)
{
	
	memcpy(dir_fisica_destino, dir_fisica_origen, tamanio);
}

void ejecutar_mov_out(int tamanio, int dir_fisica, void *datos) //chequear pagina por pid?
{
	memcpy(memoria_RAM + dir_fisica, datos, tamanio);
}
void* ejecutar_mov_in(int tamanio, int dir_fisica)
{
	void* datos = malloc(tamanio);
	memcpy(datos, memoria_RAM + dir_fisica, tamanio);

	void* datos_aux = &datos;
	free(datos);
	return datos_aux;
}
/*
void enviar_instruccion_a_cpu(t_pcb *pcb, int socket)
{

	if (list_size(lista_de_instrucciones_por_proceso) < 1)
	{
		printf("No se cargaron pcbs en memoria: \n");
		return 0;
	}

	for (int i = 0; i <= list_size(lista_de_instrucciones_por_proceso); i++)
	{

		t_instrucciones_por_proceso *instrucciones_proceso = list_get(lista_de_instrucciones_por_proceso, i);
		if (instrucciones_proceso->pid == pcb->pid)
		{

			int pc = pcb->program_counter;
			char *instruccion = list_get(instrucciones_proceso->lista_de_instrucciones, pc);

			if (strlen(instruccion) != 0)
			{
				enviar_instruccion(instruccion, socket);
				log_info(logger_memoria, "INSTRUCCION ENVIADA A CPU:  %s   PROGRAM_COUNTER:%d\n", instruccion, pc);
			}
			else
			{
				log_error(logger_memoria, "No hay instruccion para el pcb:  %d\n", instrucciones_proceso->pid);
			}

			return;
		}
	}
	log_info(logger_memoria, "Error, no hay pid cargado en memoria \n");
}
*/
/*
int buscar_marco(int pid, int num_pagina)
{
	int marco = -1;
	TablaDePaginasPorProceso *tabla_pag_proceso = buscar_tabla_por_pid(lista_de_tablas_de_paginas_por_proceso, pid);
	marco = tabla_pag_proceso->tabla_paginas->pagina[num_pagina].numero_de_marco;
	return marco;
}*/

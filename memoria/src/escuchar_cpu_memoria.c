#include "escuchar_cpu_memoria.h"
#include "iniciar_memoria.h"

void escuchar_mensajes_cpu_memoria(){
	bool desconexion_cpu_memoria = 0;

	while (!desconexion_cpu_memoria){

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
		switch (cod_op){

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
		case CPU_CONSULTA_FRAME:
			usleep(retardo_respuesta);
			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			int pagina = extraer_int_del_buffer(buffer);
			
			marco = obtener_marco(pid, pagina);

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
			log_info(logger_memoria, "MEMORIA_RESIZE");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			int nuevo_tamano = extraer_int_del_buffer(buffer);

			usleep(retardo_respuesta);

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
			int datos = extraer_int_del_buffer(buffer);
			
			printf("\n### Llega del MOV_OUT -> Datos a escribir en memoria: %i #####\n\n", datos);

			void* datos_aux = malloc(sizeof(int)); // Utilizado para la conversión
			*(int *)datos_aux = datos; // Transforma un int en un void*

			ejecutar_mov_out(tamanio, dir_fisica, pid, datos_aux);
			usleep(retardo_respuesta);
			
			print_lista_de_frames("lista_de_frames_resize_MOV_OUT.txt");
        	print_lista_procesos("lista_de_procesos_resize_MOV_OUT.txt");
			print_memoria_RAM("contenido_memoria_RAM_MOV_OUT.txt");

			free(datos_aux); // Libera el void pedido para la conversión

			destruir_buffer(buffer);
			break;
		case MEMORIA_MOV_IN: //ESCRITURA
			log_info(logger_memoria, "MOV_IN");

			buffer = recibir_buffer_completo(fd_cpu);
			pid = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			dir_fisica = extraer_int_del_buffer(buffer);
			
			buffer_a_enviar = crear_buffer();

			void* datos_a_devolver = ejecutar_mov_in(tamanio, dir_fisica, pid);

			// ES UN PRINT PARA VER LOS DATOS A ENVIAR EN BYTES POR EL VOID*
			if(datos_a_devolver != NULL){
				printf("##### DATOS A DEVOLVER DEL MOV_IN: #####\n");
				unsigned char* byte_datos = (unsigned char*)datos_a_devolver;
				for(int i = 0; i < tamanio; i++){
					printf("byte %d: %02X\n", i, byte_datos[i]);
				}
				printf("\n");
			}
			// --------------------------------------------------------------

			if(datos_a_devolver == NULL){
				log_error(logger_memoria, "El proceso no tiene suficientes paginas asignadas para leer %i bytes \n", tamanio);
				cargar_int_al_buffer(buffer_a_enviar, -1);
				paquete = crear_paquete(MEMORIA_ERROR, buffer_a_enviar);
			}
			else{
				cargar_datos_al_buffer(buffer_a_enviar, datos_a_devolver, tamanio);
				paquete = crear_paquete(MEMORIA_MOV_IN, buffer_a_enviar);
			}
			//ejemplo_MOV_IN();
			
			//cargar_int_al_buffer(pid); CREO QUE NO ES NECESARIO ENVIARLO
			
			//paquete = crear_paquete(MEMORIA_MOV_IN, buffer_a_enviar); //SI COLISIONA CAMBIAR COD_OP
			enviar_paquete(paquete, fd_cpu);
			usleep(retardo_respuesta);
			eliminar_paquete(paquete);
			free(datos_a_devolver);

			print_lista_de_frames("lista_de_frames_MOV_IN.txt");
        	print_lista_procesos("lista_de_procesos_MOV_IN.txt");
			print_memoria_RAM("contenido_memoria_RAM_MOV_IN.txt");
			break;
		case MEMORIA_COPY_STRING: //DESDE CPU SE HACE DICHA LOGICA. SOLICITAS UNA LECTURA, CON EL DATO QUE TE DEVUELVE, REALIZAR UNA ESCRITURA.
			log_info(logger_memoria, "COPY_STRING");

			buffer = recibir_buffer_completo(fd_cpu);

			pid = extraer_int_del_buffer(buffer);
			tamanio = extraer_int_del_buffer(buffer);
			int dir_fisica_origen = extraer_int_del_buffer(buffer); // Registro SI
			int dir_fisica_destino = extraer_int_del_buffer(buffer); // Registro DI
			
			void* aux = ejecutar_mov_in(tamanio, dir_fisica_origen, pid);
			ejecutar_mov_out(tamanio, dir_fisica_destino, pid, aux);

			print_memoria_RAM("contenido_memoria_COPY_STRING.txt");

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

void ejecutar_mov_out(int tamanio, int dir_fisica, int pid, void *datos){
	int indice_de_marco = 0;
	
	if(!validar_espacio_de_memoria(pid, dir_fisica, tamanio, &indice_de_marco)){
		return;
	}
	
	Proceso* proceso_actual = buscar_proceso(lista_procesos, pid);
	int base = 0;
	int pagina_actual = list_get(proceso_actual->tabla_paginas, indice_de_marco);

	printf("\n\nPAGINA ACTUAL: %i\n\n", pagina_actual);

	while(base <= tamanio){

		int direccion_fin_pagina = pagina_actual * tam_pagina + tam_pagina;
		int espacio_libre_en_pagina = direccion_fin_pagina - dir_fisica;
		int resto_de_escritura = tamanio - base;

		printf("------------\n");
		printf("Direccion fin de pagina: %i\n", direccion_fin_pagina);
		printf("Espacio libre en pagina: %i\n", espacio_libre_en_pagina);
		printf("Resto de escritura: %i\n", resto_de_escritura);
		printf("Resto de TAMANIO: %i\n", tamanio);
		printf("------------\n");

		// el tamanio restante que queda por escribir/leer sea igual o mayor --> hacer el memcpy || si es menor tendriamos que poner el tamanio de lo que resta escribir

		// ES UN PRINT PARA VER LOS DATOS A ENVIAR EN BYTES POR EL VOID*
		if(datos != NULL){
			printf("##### DATOS LEIDOS DEL STDIN_READ: #####\n");
			unsigned char* byte_datos = (unsigned char*)datos;
			for(int i = 0; i < tamanio; i++){
				printf("byte %d: %02X\n", i, byte_datos[i]);
			}
			printf("\n");
		}

		if(resto_de_escritura >= espacio_libre_en_pagina){
			printf("\nMEMCOPY IF \n");
			memcpy(memoria_RAM + dir_fisica, datos + base, espacio_libre_en_pagina);
			base += espacio_libre_en_pagina;
			printf("\nPOST-MEMCOPY IF \n");
		}else{
			printf("\nMEMCOPY ELSE \n");
			memcpy(memoria_RAM + dir_fisica, datos + base, resto_de_escritura);
			base += resto_de_escritura;
			printf("\nPOST-MEMCOPY ELSE \n");
			print_memoria_RAM("contenido_memoria_RAM_STDIN_READ.txt");
			return;
		}
		indice_de_marco++;

		printf("------------\n");
		printf("Indice de marco: %i\n", indice_de_marco);
		printf("Tamaño de tabla de paginas del proceso: %i\n", list_size(proceso_actual->tabla_paginas));
		printf("------------\n");

		if(indice_de_marco >= list_size(proceso_actual->tabla_paginas)){

			log_error(logger_memoria, "El proceso no tiene suficientes paginas asignadas para escribir %i bytes desde la direccion especificada INDICE_ERRONEO\n", tamanio);
			return NULL;
		}

		pagina_actual = list_get(proceso_actual->tabla_paginas, indice_de_marco);

		if(pagina_actual == NULL){
			log_error(logger_memoria, "El proceso no tiene suficientes paginas asignadas para escribir %i bytes desde la direccion especificada, PAG NULL\n", tamanio);
			return;
		}

		dir_fisica = pagina_actual * tam_pagina;

		printf("------------\n");
		printf("Direccion fisica: %i\n", dir_fisica);
		printf("------------\n");
	}
}

void* ejecutar_mov_in(int tamanio, int dir_fisica, int pid){
	int indice_de_marco;
	void* datos = calloc(tamanio, 1);

	if(!validar_espacio_de_memoria(pid, dir_fisica, tamanio, &indice_de_marco)){
		free(datos);
		return NULL;
	}

	Proceso* proceso_actual = buscar_proceso(lista_procesos, pid);
	int base = 0;
	int pagina_actual = list_get(proceso_actual->tabla_paginas, indice_de_marco);

	while(base <= tamanio){
		int direccion_fin_pagina = pagina_actual * tam_pagina + tam_pagina;
		int espacio_libre_en_pagina = direccion_fin_pagina - dir_fisica;
		int resto_de_lectura = tamanio - base;

		printf("------------\n");
		printf("Direccion fin de pagina: %i\n", direccion_fin_pagina);
		printf("Espacio libre en pagina: %i\n", espacio_libre_en_pagina);
		printf("Resto de lectura: %i\n", resto_de_lectura);
		printf("############\n");
		//printf("Datos: %d\n", *(int*)datos);
		printf("Base: %i\n", base);
		printf("------------\n");

		// el tamanio restante que queda por escribir/leer sea igual o mayor --> hacer el memcpy || si es menor tendriamos que poner el tamanio de lo que resta escribir
		if(resto_de_lectura >= espacio_libre_en_pagina){

			// ES UN PRINT PARA VER LOS DATOS ENVIADOS EN BYTES POR EL VOID*
			unsigned char* byte_memoria = (unsigned char*)memoria_RAM;
			printf("\nContenido de memoria desde la dirección física %d:\n", dir_fisica);
			for(int i = 0; i < tamanio; i++){
				printf("byte %d: %02X\n", dir_fisica + i, byte_memoria[dir_fisica + i]);
			}
			printf("\n");
			// --------------------------------------------------------------

			memcpy(datos + base, memoria_RAM + dir_fisica, espacio_libre_en_pagina);
			//memcpy(1, memoria_RAM, espacio_libre_en_pagina);
			base += espacio_libre_en_pagina; // Porque más uno?
		}
		else{

			// ES UN PRINT PARA VER LOS DATOS ENVIADOS EN BYTES POR EL VOID*
			unsigned char* byte_memoria = (unsigned char*)memoria_RAM;
			printf("\nContenido de memoria desde la dirección física %d:\n", dir_fisica);
			for(int i = 0; i < tamanio; i++){
				printf("byte %d: %02X\n", dir_fisica + i, byte_memoria[dir_fisica + i]);
			}
			printf("\n");
			// --------------------------------------------------------------

			memcpy(datos + base, memoria_RAM + dir_fisica, resto_de_lectura);
			base += resto_de_lectura;

			return datos;
		}

		indice_de_marco++;

		if(indice_de_marco >= list_size(proceso_actual->tabla_paginas)){
			return NULL;
		}

		pagina_actual = list_get(proceso_actual->tabla_paginas, indice_de_marco);

		if(pagina_actual == NULL){
			return NULL;
		}

		dir_fisica = pagina_actual * tam_pagina;
	}

	return datos;
}

bool validar_espacio_de_memoria(int pid, int dir_fisica, int tam, int* indice_de_marco){ // CAPAZ ROMPE

	bool contiene_direccion = false;

	Proceso* proceso_actual = buscar_proceso(lista_procesos, pid);
	if(proceso_actual == NULL){
		log_error(logger_memoria, "No existe proceso con PID: %i", pid);
		return contiene_direccion;
	}
	
	for(int i = 0; i < list_size(proceso_actual->tabla_paginas); i++){

		int pagina_actual = list_get(proceso_actual->tabla_paginas, i);
		int inicio_marco_actual = pagina_actual * tam_pagina;
		if(inicio_marco_actual <= dir_fisica && inicio_marco_actual + tam_pagina >= dir_fisica){ //DIRECCION DENTRO DE LAS PAGS DEL PROCESO
			contiene_direccion = true;
			*indice_de_marco = i;
			return contiene_direccion;
		}
	}
	return contiene_direccion;
}

// ---------- FUNCIONES PARA PROBAR LA ESCRITURA - NO ES PARTE DEL TP ----------
void MOV_IN(int src_addr, int dest_addr, int size){
    // Verificar que las direcciones y el tamaño sean válidos
    if(src_addr < 0 || src_addr >= tam_memoria || 
        dest_addr < 0 || dest_addr >= tam_memoria || 
        size < 0 || src_addr + size > tam_memoria || dest_addr + size > tam_memoria){
        printf("Error: Direcciones o tamaño inválidos\n");
        return;
    }

    // Mover los datos desde la dirección de origen a la dirección de destino
    memcpy((char *)memoria_RAM + dest_addr, (char *)memoria_RAM + src_addr, size);
}

// ---------- FUNCIONES PARA PROBAR LA ESCRITURA - NO ES PARTE DEL TP ----------
void ejemplo_MOV_IN(){
    // Inicializar algunos datos en la memoria
    char *data = "Hello, World!";
    memcpy(memoria_RAM, data, strlen(data) + 1);  // Copiar "Hello, World!" a la memoria desde el inicio

    // Imprimir contenido inicial de la memoria
    printf("Contenido inicial de la memoria:\n");
    printf("%s\n", (char *)memoria_RAM);

    // Mover 10 bytes desde la dirección 0 a la dirección 100
    MOV_IN(0, 100, 10);

    // Imprimir el contenido de la memoria después del movimiento
    printf("Contenido de la memoria después de MOV_IN:\n");
    printf("Desde dirección 0: %s\n", (char *)memoria_RAM);
    printf("Desde dirección 100: %.*s\n", 10, (char *)memoria_RAM + 100);
}
// -----------------------------------------------------------------------------

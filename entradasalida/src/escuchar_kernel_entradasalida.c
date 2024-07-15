#include "escuchar_kernel_entradasalida.h"
#include <readline/readline.h>
#include <dirent.h>


void escuchar_instrucciones_generica(){
	printf("escuchar1\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case IO_GEN_SLEEP_FS:
				
				t_buffer* buffer;
				buffer = recibir_buffer_completo(fd_kernel);
				
				int pid = extraer_int_del_buffer(buffer);
				int unidades = extraer_int_del_buffer(buffer);

				usleep(unidades * tiempo_unidad_trabajo);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				t_buffer* buffer_kernel;
				buffer_kernel = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer_kernel);
				free(buffer_kernel);

				buffer_kernel = crear_buffer();
				cargar_int_al_buffer(buffer_kernel, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer_kernel);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}


void escuchar_instrucciones_stdin(){
	printf("escuchar2\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer;
			int pid;
			case IO_STDIN_READ_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				printf("\nIngrese hasta %i caracteres\n", reg_tamanio);
				char* input = readline("> ");
				char* mensaje;
				memcpy(mensaje, input, reg_tamanio);

				solicitar_almacen_memoria(reg_direccion, mensaje, IO_STDIN_READ_FS);

				// ESPERAR ERROR MEMORIA?

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_stdout(){
	printf("escuchar3\n");
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer = crear_buffer();
			int pid;
			case IO_STDOUT_WRITE_FS:
				printf("a\n");
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				solicitar_lectura_memoria(reg_direccion, reg_tamanio, IO_STDOUT_WRITE_FS);

				sem_wait(&sem_stdout);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
				
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;

			default: // La instruccion es incorrecta
				printf("b\n");
				buffer = recibir_buffer_completo(fd_kernel);
				printf("c\n");
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				//buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_dialfs(){
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer = crear_buffer();
			int pid;
			int reg_direccion;
			int reg_tamanio;
			int reg_puntero_archivo;
			void* aux;
			char* nombre;
			t_config* config;
			case IO_FS_CREATE_FS:

				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

				// Chequear si hay lugar en bitmap
				int direccion = buscar_lugar_bitmap(1);
				// Asignar lugar en bitmap
				bitarray_set_bit(bitmap, direccion);
				// Crear archivo metadata
				FILE* archivo = fopen(nombre, "w");
				fclose(archivo);

				config = config_create(nombre);

				config_set_value(config, "BLOQUE_INICIAL", string_itoa(direccion));
				config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(0));
				config_save(config);
				// Sincronizar
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				config_destroy(config);
				break;

			case IO_FS_DELETE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

				liberar_archivo_bitmap(nombre);
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC); //dudoso, pero creo que va

				remove(nombre);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				config_destroy(config);
				break;

			case IO_FS_WRITE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

				reg_direccion = extraer_int_del_buffer(buffer);
				reg_tamanio = extraer_int_del_buffer(buffer);
				reg_puntero_archivo = extraer_int_del_buffer(buffer);

				solicitar_lectura_memoria(reg_direccion, reg_tamanio, IO_STDOUT_WRITE_FS);
				sem_wait(&sem_fs_write);

				if(!verificar_escritura_archivo(nombre, reg_tamanio, reg_puntero_archivo)){
					log_error(logger_entradasalida, "Se intenta escribir por fuera del tamanio del archivo\n");
					goto error_io;
				}
				
				aux = bloques_dat + reg_puntero_archivo;

				memcpy(aux, datos, reg_tamanio); 

				msync(bloques_dat, block_size*block_count, MS_SYNC);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				config_destroy(config);

				break;
			
			case IO_FS_READ_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

				reg_direccion = extraer_int_del_buffer(buffer);
				reg_tamanio = extraer_int_del_buffer(buffer);
				reg_puntero_archivo = extraer_int_del_buffer(buffer);

				if(!verificar_escritura_archivo(nombre, reg_tamanio, reg_puntero_archivo)){
					log_error(logger_entradasalida, "Se intenta leer por fuera del tamanio del archivo\n");
					goto error_io;
				}

				char* leido = malloc(reg_tamanio);
				aux = bloques_dat + reg_puntero_archivo;

				memcpy(leido, aux, reg_tamanio);

				solicitar_almacen_memoria(reg_direccion, leido, IO_FS_READ_FS);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				config_destroy(config);

				break;

			case IO_FS_TRUNCATE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);
				int nuevo_tamanio = extraer_int_del_buffer(buffer);

				config = config_create(nombre);
				if(config == NULL){
					log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
				}
				int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");
				int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
				//TODO
				//Queremos agrandar o achicar?

				int cantidad_nueva_de_bloques = redondear_up(nuevo_tamanio, block_size);
				int cantidad_actual_de_bloques = redondear_up(tamanio_archivo, block_size);

				int diferencia = cantidad_actual_de_bloques - cantidad_nueva_de_bloques;

				if (cantidad_actual_de_bloques > cantidad_nueva_de_bloques){
					//si achicamos solo achicamos el archivo de metadata y marcamos los bloques restantes libres en el bitmap
					
					config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
					config_save(config);

					for(int i = 1; i < diferencia; i++){
						bitarray_clean_bit(bitmap, redondear_up(inicio_archivo+nuevo_tamanio, block_size) + i );
					}
				}else{
					// si queremos agrandar
					diferencia = abs(diferencia);
					if (buscar_lugar_bitmap(diferencia) < 0){
						log_error(logger_entradasalida, "No hay espacio suficiente para truncar este archivo");
						break;
					}
				// verificar que haya espacio
				// chequeamos si entra
				int fin_archivo = inicio_archivo + tamanio_archivo;
				for(int i = 1; i < diferencia; i++){
					if(bitarray_test_bit(bitmap, fin_archivo + i) == 1){
						compactar(nombre, config);
						break;
					}
				}
				// si no entra, hay que compactar
					// leemos el directorio "raiz"
					// por cada config del metadata agregamos una entrada a una lista
					// cambiamos tamanio archivo truncado
					// teniendo el inicio y tamanio de todos los archivos copiamos todo a la lista
					// sobreescribimos el archivo de bloques original, modificando los archivos de metadata y el bitmap
				}
				//
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);
				break;
				
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;

			default: // La instruccion es incorrecta
				error_io:
				printf("b\n");
				buffer = recibir_buffer_completo(fd_kernel);
				printf("c\n");
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				//buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}



void notificar_fin(int fd_kernel, int pid){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, pid);

	t_paquete* paquete = crear_paquete(FIN_IO, buffer);
	enviar_paquete(paquete, fd_kernel);

	eliminar_paquete(paquete);
	free(buffer);
}

void solicitar_lectura_memoria(int direccion, int tamanio, op_code cod_op){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_int_al_buffer(buffer, tamanio);

	t_paquete* paquete = crear_paquete(cod_op, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	//free(buffer);
}

void solicitar_almacen_memoria(int direccion, char* mensaje, op_code cod_op){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_string_al_buffer(buffer, mensaje);

	t_paquete* paquete = crear_paquete(cod_op, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	//free(buffer);
}

int buscar_lugar_bitmap(int tamanio){
	int i = 0;
	int contador_libres = 0;
	int contador_libres_continuo = 0;
	while(i < block_count){
		if(bitarray_test_bit(bitmap, i) == 0){
			contador_libres ++;
			contador_libres_continuo ++;
			if (contador_libres_continuo >= tamanio){return i - tamanio;}
		}
		if(bitarray_test_bit(bitmap, i) == 1){
			contador_libres_continuo = 0;
		}

		i ++;
	}
	
	if(contador_libres >= tamanio){
		// TODO: comprimir() -> buscar
		return -1;
	}
	else{
		// TODO: DEVUELVE ERROR, NO HAY ESPACIO.
		return -1;
	}
}

bool verificar_escritura_archivo(char* path, int reg_tamanio, int reg_puntero_archivo){
	t_config* config = config_create(path);
	
	if(config == NULL){
		log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
	}

	int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	config_destroy(config);	

	return reg_tamanio + reg_puntero_archivo <= inicio_archivo * block_size + tamanio_archivo;
}

void liberar_archivo_bitmap(char* path){
	t_config* config = config_create(path);
	
	if(config == NULL){
		log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
	}

	int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	for(int i = 0; i < redondear_up(tamanio_archivo, block_size); i++){
		bitarray_clean_bit(bitmap, inicio_archivo + i);
	}
}

void compactar(char* nombre, t_config* config){
// leemos el directorio "raiz"
	// DIR* opendir("/home/utnso/tp-2024-1c-PapaNoisette/entradasalida/");
	// por cada config del metadata agregamos una entrada a una lista
	// cambiamos tamanio archivo truncado
	// teniendo el inicio y tamanio de todos los archivos copiamos todo a la lista
	// sobreescribimos el archivo de bloques original, modificando los archivos de metadata y el bitmap
}
/*

 case FS_TRUNCATE:
            log_info(logger, "PID: %d - Operacion: FS_TRUNCATE", pid);
            uint32_t* puntero_nuevo_tamanio = ((uint32_t*) sacar_de_paquete(paquete->buffer, &desplazamiento));
            uint32_t nuevo_tamanio = *puntero_nuevo_tamanio;
            free(puntero_nuevo_tamanio);

            uint32_t cantidad_nueva_de_bloques = cantidad_bloques_segun_bytes(nuevo_tamanio, tamanio_bloque);//La cantidad de bloques en la va a quedar el archivo

            //Bloque inicial del archivo post ser truncado. Puede ser el mismo que el original o variar si se compacta el fs
            int bloque_inicial_nuevo = bloque_inicial_archivo;

            //Cantidad de bloques del archivo (previo a truncarlo)
            int cant_actual_bloques = cantidad_bloques_segun_bytes(tamanio_inicial_archivo, tamanio_bloque); 

            //Esto se podría omitir porque el asignar ya tiene la lógica de verificar si hay suficientes bloques libres, pero cortarlo ahora te ahorra de hacer muchas cosas
            //Verificas si la cantidad nueva de bloques es mayor que la cantidad total de bloques del fs -> no te importa el estado de los bloques
            if (cantidad_nueva_de_bloques > cantidad_total_de_bloques){
                    log_error(logger, "No hay espacio suficiente para hacer tal cambio, recordar que el espacio total es de %d", tamanio_bloque * cantidad_total_de_bloques);
                    break;
            }

            log_info(logger, "PID: <%d>-Truncar Archivo: <%s> Tamaño: <%d>", pid, nombre_archivo, nuevo_tamanio);

            int resultado_asignar_bloques = 0;

            if(cantidad_nueva_de_bloques > cant_actual_bloques){

                //Cantidad de bloques a agregar (se le resta la cantidad actual para saber cuántos debemos agregar)
                int cantidad_de_bloques_a_agregar = cantidad_nueva_de_bloques - 
    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, signbloques_a_agregar, ultimo_bloque_archivo);
                if(resultado_asignar_bloques == -2){
                    log_info(logger, "PID: <%d> - Inicio Compactación", pid);
                    usleep(retardo_compactacion * 1000);
                    compactar(nombres_archivos_metadata, metadata, bitmap, tamanio_bloque, cantidad_total_de_bloques, tamanio_inicial_archivo, nuevo_tamanio, bloque_inicial_archivo, bloques, path_archivo);
                    log_info(logger, "PID: <%d> - Fin Compactación", pid);
                }
                if(resultado_asignar_bloques == -1) {
                    log_error(logger, "No se pudo asignar los bloques necesarios");
                    break;
                }

            } else if(cantidad_nueva_de_bloques < cant_actual_bloques){

                int cantidad_bloques_a_liberar = cant_actual_bloques - cantidad_nueva_de_bloques;
                int primer_bloque_desde_donde_liberar = ultimo_bloque_archivo - cantidad_bloques_a_liberar; 
                liberar_bloques_desde_hasta(bitmap, primer_bloque_desde_donde_liberar, ultimo_bloque_archivo);

            }
            
            //Cambiar archivo de metadata
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, sign
            free(string_nuevo_tamanio);
            
            config_destroy(metadata);
            
            break;
        case FS_WRITE:
            log_info(logger, "PID: %d - Operacion: FS_WRITE", pid);
            char *resultado = NULL;
            uint32_t bytes_leidos = 0;

            while(paquete->buffer->size > desplazamiento){
                t_dato_paginado* dato_pagina = (t_dato_paginado*) sacar_de_paquete(paquete->buffer, &desplazamiento);//t_dato_paginado tiene dirección física y tamanio a escribir desde esa dirección física

                solicitar_texto_memoria(conexion_memoria, pid, dato_pagina->direccion_fisica, dato_pagina->tamanio);

                t_paquete *paquete_memoria = recibir_paquete(conexion_memoria);

                if(paquete_memoria->codigo_operacion != LECTURA_ESP_USUARIO) {
                    log_error(logger, "Error al recibir contenido de memoria");
                    break;
                }

                resultado = realloc(resultado, paquete_memoria->buffer->size + bytes_leidos);

    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, sign

                eliminar_paquete(paquete_memoria);

                free(dato_pagina);
            }
    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, sign

            memcpy(bloques + primer_byte_a_operar, resultado, tamanio_a_operar);
            msync(bloques, cantidad_total_de_bloques * tamanio_bloque, MS_SYNC);

            log_info(logger, "PID: <%d> - Escribir Archivo: <%s> - Tamaño a Escribir: <%d> - Puntero Archivo: <%d>", pid, nombre_archivo, tamanio_a_operar, desplazamiento_sobre_archivo);
            log_debug(logger, "IO_FS_WRITE TEXTO ESCRITO EN ARCHIVO %s ", resultado);

            free(resultado);

            config_destroy(metadata);
            
            break;
    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, sign
int asignar_bloques(t_bitarray* bitmap, int cantidad_bloques_a_agregar, int bloque_inicial){//Bloque inicial es bloque desde donde voy a asignar y no tu último bloque

    if(cantidad_bloques_a_agregar == 0)
        return bloque_inicial;

    int bloque_inicial_nuevo = bloque_inicial;
    int bloque_desde_donde_extender = buscar_bloque_con_n_bloques_libres_contiguos(bitmap, cantidad_bloques_a_agregar, bloque_inicial);

    if(
        bloque_desde_donde_extender != -1 &&
        bloque_desde_donde_extender == bloque_inicial // Si no son iguales, significa que no se puede extender desde el bloque inicial
    ){
        extender_bloques(bitmap, cantidad_bloques_a_agregar , bloque_inicial);
    } else {
        int cantidad_bloques_sin_asignar = cantidad_bloques_libres(bitmap);
        if(cantidad_bloques_sin_asignar >= cantidad_bloques_a_agregar){
           return -2;
           //Acá el bloque inicial es el nuevo bloque inicial -> 
        } else {
           return -1;
        }
    }

    return bloque_inicial_nuevo;
}

void compactar(t_list* archivos_metadata, t_config* metadata_causa_la_compactacion, t_bitarray* bitmap, 
                int tamanio_bloque, int cantidad_bloques, int tamanio_actual_archivo, int tamanio_nuevo, int bloque_inicial, void* bloques, char* path_archivo){
    
    
    eliminar_nombre_si_es(archivos_metadata, path_archivo);

    //Buffer para guardar los datos del archivo a agrandar
    void* buffer_archivo = malloc(tamanio_actual_archivo);

    //Guardas los datos actuales
    memcpy(buffer_archivo, bloques + bloque_inicial * tamanio_bloque, tamanio_actual_archivo);

    //Marcas como vacíos los bloques que ocupaba en el bitmap
    liberar_bloques_desde_hasta(bitmap, bloque_inicial, ultimo_bloque(bloque_inicial, tamanio_actual_archivo, tamanio_bloque));

    //Buffer para guardar todos la info de los bloques de datos que no son el archivo a agrandar
    void* buffer_auxiliar = malloc(tamanio_bloque * cantidad_bloques);

    uint32_t desplazamiento_en_bloques = 0;
    
    

    for(int i=0; i < list_size(archivos_metadata); i++){
        char* nombre_archivo_recorrido = list_get(archivos_metadata, i);

        t_config* metadata_recorrido = config_create(nombre_archivo_recorrido);

        if(metadata_recorrido == NULL){
            log_info(logger, "El archivo de metada %s no existe", nombre_archivo_recorrido);
            continue;
        }

        int bloque_inicial_metadata_recorrido = config_get_int_value(metadata_recorrido, "BLOQUE_INICIAL");
        int tamanio_metadata_recorrido = config_get_int_value(metadata_recorrido, "TAMANIO_ARCHIVO");

        memcpy(buffer_auxiliar + desplazamiento_en_bloques * tamanio_bloque, bloques + bloque_inicial_metadata_recorrido * tamanio_bloque , tamanio_metadata_recorrido);

        char* string_desplazamiento_en_bloques_parcial = string_itoa(desplazamiento_en_bloques);
        config_set_value(metadata_recorrido, "BLOQUE_INICIAL", string_desplazamiento_en_bloques_parcial);
        free(string_desplazamiento_en_bloques_parcial);
        config_save(metadata_recorrido);
        config_destroy(metadata_recorrido);
        
        desplazamiento_en_bloques += cantidad_bloques_segun_bytes(tamanio_metadata_recorrido, tamanio_bloque);

    }
    //Hasta acá tenemos todos los que no son el archivo por el que compacté en el buffer auxiliar compactados
    char* string_desplazamiento_en_bloques = string_itoa(desplazamiento_en_bloques);
    config_set_value(metadata_causa_la_compactacion, "BLOQUE_INICIAL", string_desplazamiento_en_bloques);
    free(string_desplazamiento_en_bloques);

    memcpy(buffer_auxiliar + desplazamiento_en_bloques * tamanio_bloque, buffer_archivo ,tamanio_actual_archivo);
    desplazamiento_en_bloques += cantidad_bloques_segun_bytes(tamanio_actual_archivo, tamanio_bloque);

    for(uint32_t j = 0; desplazamiento_en_bloques > j; j++){
        bitarray_set_bit(bitmap, j);
    }
    for(uint32_t k = desplazamiento_en_bloques; cantidad_bloques >= k; k++){
        bitarray_clean_bit(bitmap, k);
    }

    extender_bloques(bitmap, cantidad_bloques_segun_bytes(tamanio_nuevo, tamanio_bloque) - cantidad_bloques_segun_bytes(tamanio_actual_archivo, tamanio_bloque),
                    desplazamiento_en_bloques);
    
    //SINCRONIZAR ARCHIVO BITMAP??

    memcpy(bloques, buffer_auxiliar, tamanio_bloque * cantidad_bloques);
    msync(bloques, cantidad_bloques * tamanio_bloque, MS_SYNC);

    //SINCRONIZAR ARCHIVO BLOQUES??

    char* string_tamanio_nuevo = string_itoa(tamanio_nuevo);
    config_set_value(metadata_causa_la_compactacion, "TAMANIO_ARCHIVO", string_tamanio_nuevo);
    free(string_tamanio_nuevo);
    
    config_save(metadata_causa_la_compactacion);
    char* path_archivo_duplicado = strdup(path_archivo);
    list_add(archivos_metadata, path_archivo_duplicado);
    

    free(buffer_archivo);
    free(buffer_auxiliar);
}
*/
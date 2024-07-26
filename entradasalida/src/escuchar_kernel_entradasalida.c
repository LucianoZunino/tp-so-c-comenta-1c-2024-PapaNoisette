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
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");

				t_buffer* buffer_kernel;
				buffer_kernel = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer_kernel);
				free(buffer_kernel);

				buffer_kernel = crear_buffer();
				cargar_int_al_buffer(buffer_kernel, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer_kernel);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
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
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");
				
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
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
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");

				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				//buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
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

				usleep(tiempo_unidad_trabajo);

				char* path;
				strcpy(path, tomar_nombre_devolver_path(nombre));
				eliminar_segun(nombre);
				list_add(archivos_metadata, path);

				// Chequear si hay lugar en bitmap
				int direccion = buscar_lugar_bitmap(1);
				// Asignar lugar en bitmap
				bitarray_set_bit(bitmap, direccion);
				// Crear archivo metadata
				FILE* archivo = fopen(path, "w");
				fclose(archivo);

				config = config_create(path);

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

				usleep(tiempo_unidad_trabajo);

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

				usleep(tiempo_unidad_trabajo);

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

				usleep(tiempo_unidad_trabajo);

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

				usleep(tiempo_unidad_trabajo);

				config = config_create(tomar_nombre_devolver_path(nombre));
				if(config == NULL){
					log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
					break;
				}
				int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");
				int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
				//TODO
				//Queremos agrandar o achicar?

				int cantidad_nueva_de_bloques = redondear_up(nuevo_tamanio, block_size);
				int cantidad_actual_de_bloques = redondear_up(tamanio_archivo, block_size);

				int diferencia = cantidad_actual_de_bloques - cantidad_nueva_de_bloques;

				if(diferencia == 0){
					log_info(logger_entradasalida, "Archivo de mismo tamanio en bloques, no hace falta truncar");
					break;
				}

				if (cantidad_actual_de_bloques > cantidad_nueva_de_bloques){
					//si achicamos solo achicamos el archivo de metadata y marcamos los bloques restantes libres en el bitmap
					
					config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
					config_save(config);

					liberar_bloques_desde_hasta(inicio_archivo + cantidad_nueva_de_bloques, inicio_archivo + cantidad_actual_de_bloques);

				}else{
					// si queremos agrandar
					diferencia = abs(diferencia);

					if (buscar_lugar_bitmap(diferencia) < 0){
						log_error(logger_entradasalida, "No hay espacio suficiente para truncar este archivo");
						break;
					}
				// verificar que haya espacio
				// chequeamos si entra
					int fin_archivo = inicio_archivo + redondear_up(tamanio_archivo, block_size);
					int i = 0;
					for(i = 1; i <= diferencia; i++){
						if(bitarray_test_bit(bitmap, fin_archivo + i) == 1){
							compactar(nombre, config, nuevo_tamanio);
							break;
						}
					}

					if (i >= diferencia){
						config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
						config_save(config);
					}

				}
				//
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);
				break;
				
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;

			default: // La instruccion es incorrecta
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");
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

bool verificar_escritura_archivo(char* nombre, int reg_tamanio, int reg_puntero_archivo){
	t_config* config = config_create(tomar_nombre_devolver_path(nombre));
	
	if(config == NULL){
		log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
	}

	int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	config_destroy(config);	

	return reg_tamanio + reg_puntero_archivo <= inicio_archivo * block_size + tamanio_archivo;
}

void liberar_archivo_bitmap(char* nombre){
	t_config* config = config_create(tomar_nombre_devolver_path(nombre));
	
	if(config == NULL){
		log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
	}

	int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	for(int i = 0; i < redondear_up(tamanio_archivo, block_size); i++){
		bitarray_clean_bit(bitmap, inicio_archivo + i);
	}
}

void liberar_bloques_desde_hasta(inicio, fin){
	for(int i = 1; i <= fin - inicio; i++ ){
		bitarray_clean_bit(bitmap, inicio + i);
	}
}

void eliminar_segun(char* nombre){
	for(int i = 0; i<list_size(archivos_metadata); i++){
		char* path = list_get(archivos_metadata, i);
		if(strcmp(path, tomar_nombre_devolver_path(nombre))){
			list_remove(archivos_metadata, i);
			break;
		}
	}
}

void compactar(char* nombre, t_config* config, int nuevo_tamanio){
	// ELIMINAMOS EL ARCHIVO A AGRANDAR, LO AGREGAMOS AL FINAL
	eliminar_segun(nombre);

	int bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	// Actualizamos el archivo de metadata del archivo a truncar
	config_set_value(config, "TAMANIO_ARCHIVO", nuevo_tamanio);
	config_save(config);
	
	// Guardamos los datos del archivo
	/*
	void* buffer_archivo = malloc(nuevo_tamanio);
	memcpy(buffer_archivo, bloques_dat + bloque_inicial * block_size, nuevo_tamanio);
	*/

	// Liberamos todos los bloques
	liberar_bloques_desde_hasta(-1, block_count);

	// Creamos un buffer para guardar los datos del resto de los archivos
	void* buffer_auxiliar = malloc(block_size * block_count);

	// Agregamos el archivo a truncar al final de la lista
	char* path_del_nombre = tomar_nombre_devolver_path(nombre);
	list_add(archivos_metadata, path_del_nombre);
	
	int bloques_desplazados = 0;

	for(int i = 0; i < list_size(archivos_metadata); i++){
		// Tomamos cada arhivo de la lista de archivos en el FS
		char* path_metadata = list_get(archivos_metadata, i);

		// Abrimos el config de cada archivo en el FS
		t_config* config_actual = config_create(path_metadata);

		if(config_actual == NULL){
            log_info(logger_entradasalida, "El archivo de metada %s no existe", path_metadata);
            continue;
        }

		// Leemos el config actual
		int bloque_inicial_actual = config_get_int_value(config, "BLOQUE_INICIAL");
		int tamanio_archivo_actual = config_get_int_value(config, "TAMANIO_ARCHIVO"); 

		

		// Copiamos datos del archivo actual
		if( strcmp(path_metadata, path_del_nombre) != 0){ // Verificamos que los datos a leer no pertenecen al archivo truncado en caso de que el nuevo tamanio se pase del espacio de memoria
			memcpy(buffer_auxiliar + bloques_desplazados * block_size, bloques_dat + bloque_inicial_actual * block_size, tamanio_archivo_actual);
		}else{
			memcpy(buffer_auxiliar + bloques_desplazados * block_size, bloques_dat + bloque_inicial_actual * block_size, tamanio_archivo);
		}
		char* desplazamiento_actual = string_itoa(bloques_desplazados);
        config_set_value(config_actual, "BLOQUE_INICIAL", desplazamiento_actual);
        free(desplazamiento_actual);
		config_save(config_actual);
		config_destroy(config_actual);

		bloques_desplazados += redondear_up(tamanio_archivo_actual, block_size);
	}

	for(int i = 0; i < bloques_desplazados; i++){
		bitarray_set_bit(bitmap, i);
	}



	// Sincronizamos archivo de bloques
	memcpy(bloques_dat, buffer_auxiliar, block_count * block_size);
	msync(bloques_dat, block_size*block_count, MS_SYNC);

	// Sincronizamos bitmap
	msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);

	config_destroy(config);
	usleep(retraso_compactacion);
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
*/


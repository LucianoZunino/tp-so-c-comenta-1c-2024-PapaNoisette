#include "escuchar_kernel_entradasalida.h"
#include <readline/readline.h>
#include <dirent.h>
#include <math.h>

void escuchar_instrucciones_generica(){
	
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		
		switch(cod_op){
			case IO_GEN_SLEEP_FS: // LLEGA COD_OP 8 ERRONEO Y TIENE QUE LLEGAR UN 9
				
				t_buffer* buffer = crear_buffer();
				buffer = recibir_buffer_completo(fd_kernel);
				
				int pid = extraer_int_del_buffer(buffer);
				int unidades = extraer_int_del_buffer(buffer);

                log_info(logger_entradasalida, "PID: %i - Operacion: IO_GEN_SLEEP", pid);

				
				usleep(unidades * tiempo_unidad_trabajo * 1000);
				notificar_fin(fd_kernel, pid);
				destruir_buffer(buffer);
				
				break;
			case NUEVA_CONEXION_IO: // Puede ser qu e el ok llegue x acá en lugar de recibirlo en el recibir_ok()

				printf("\n\nRecibir ok?\n\n");
				
				break;
			default: // La instruccion es incorrecta
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");
				printf("\n\n\n COD_OP2: %i\n\n\n", cod_op);

				t_buffer* buffer_kernel;
				buffer_kernel = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer_kernel);
				destruir_buffer(buffer_kernel);

				buffer_kernel = crear_buffer();
				cargar_int_al_buffer(buffer_kernel, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer_kernel);
				enviar_paquete(paquete, fd_kernel);

				printf("\n\n\n:(\n\n");
				eliminar_paquete(paquete);
				break;
		}
	}
}


void escuchar_instrucciones_stdin(){
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

				log_info(logger_entradasalida, "PID: %i - Operacion: IO_STDIN_READ", pid);

				printf("\nIngrese hasta %i caracteres\n", reg_tamanio);
				char* input;
				input = readline("\n> ");
				//char* mensaje;
				
    
    			while(reg_tamanio != string_length(input)){
        			printf("\nLA CADENA INGRESADA DEBE SER DE UNA LONGITUD == %d\n",reg_tamanio);
        			input = readline(">");
    			}
				//memcpy(mensaje, input, reg_tamanio);

				solicitar_almacen_memoria(pid, reg_direccion, input, IO_STDIN_READ_FS);

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
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer = crear_buffer();
			int pid;
			case IO_STDOUT_WRITE_FS:
				
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

                log_info(logger_entradasalida, "PID: %i - Operacion: IO_STDOUT_WRITE", pid);

				//printf("\n\nRecibido de kernel Tamanio: %i --- Direccion Fisica: %i\n\n", reg_tamanio, reg_direccion);

				solicitar_lectura_memoria(pid, reg_direccion, reg_tamanio, IO_STDOUT_WRITE_FS);

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
			int inicio_archivo;

			case IO_FS_CREATE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);

                log_info(logger_entradasalida, "PID: <%i> - Operacion: IO_FS_CREATE", pid);
				usleep(tiempo_unidad_trabajo);
				log_info(logger_entradasalida,"PID: <%i> - Crear Archivo: <%s>",pid, nombre);
				
				char* path = string_duplicate(tomar_nombre_devolver_path(nombre));
				
				if(eliminar_segun(path) != -1){ // Si el archivo ya existe y se crea otro con el mismo nombre se debería eliminar
					liberar_archivo_bitmap(nombre);
				}
				
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
				//config_destroy(config); //HAY QUE COMENTAR ESTE? O EL DE DELETE?
				break;

			case IO_FS_DELETE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);
				char* path1 = tomar_nombre_devolver_path(nombre);

				log_info(logger_entradasalida, "PID: <%i> - Operacion: IO_FS_DELETE", pid);
				usleep(tiempo_unidad_trabajo);
				log_info(logger_entradasalida,"PID: <%i> -  Eliminar Archivo: <%s>",pid, nombre );

				liberar_archivo_bitmap(nombre);
				
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC); //dudoso, pero creo que va
				
				if(remove(path1) != 0){
					log_error(logger_entradasalida, "No se pudo encontrar el archivo %s", nombre);
				}

				// devolver
				notificar_fin(fd_kernel, pid);
				destruir_buffer(buffer);
				
				//config_destroy(config);
				
				break;

			case IO_FS_WRITE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);
				reg_direccion = extraer_int_del_buffer(buffer);
				reg_tamanio = extraer_int_del_buffer(buffer);
				reg_puntero_archivo = extraer_int_del_buffer(buffer);

				printf("\n----NUMERO DE PROCESO: %i, REG_DIRECCCION: %i, REG_TAMAÑO: %i, REG_PUNTERO_ARCHIVO: %i-----\n", pid, reg_direccion, reg_tamanio, reg_puntero_archivo);

				log_info(logger_entradasalida, "PID: <%i> - Operacion: IO_FS_WRITE", pid);
				usleep(tiempo_unidad_trabajo);
                log_info(logger_entradasalida,"PID: <%i> -  Escribir Archivo: <%s> -  Tamaño a Escribir: <%i> - Puntero Archivo: <%i>",pid, nombre, reg_tamanio, reg_puntero_archivo);
				
				solicitar_lectura_memoria(pid, reg_direccion, reg_tamanio, IO_FS_WRITE_FS);

				config = config_create(tomar_nombre_devolver_path(nombre));

				inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
				
				sem_wait(&sem_fs_write);
				
				if(!verificar_escritura_archivo(nombre, reg_tamanio, reg_puntero_archivo)){
					log_error(logger_entradasalida, "Se intenta escribir por fuera del tamanio del archivo\n");
					goto error_io;
				}
				
				//aux = bloques_dat + reg_puntero_archivo;

				printf("\nDatos a escribir en disco: %s\n", datos);
				printf("\nEN: %i\n", inicio_archivo * block_size + reg_puntero_archivo);

				//void* datos_aux = datos;

				memcpy(bloques_dat + inicio_archivo*block_size + reg_puntero_archivo, (void*)datos, reg_tamanio);
				
				msync(bloques_dat, block_size * block_count, MS_SYNC);

				// devolver
				
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				//config_destroy(config);

				break;
			
			case IO_FS_READ_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);
				reg_direccion = extraer_int_del_buffer(buffer);
				reg_tamanio = extraer_int_del_buffer(buffer);
				reg_puntero_archivo = extraer_int_del_buffer(buffer);

				log_info(logger_entradasalida, "PID: <%i> - Operacion: IO_FS_READ", pid);
				usleep(tiempo_unidad_trabajo);
				log_info(logger_entradasalida,"PID: <%i> -   Leer Archivo: <%s> -  Tamaño: <%i> - Puntero Archivo: <%i>",pid, nombre, reg_tamanio, reg_puntero_archivo);	

				if(!verificar_escritura_archivo(nombre, reg_tamanio, reg_puntero_archivo)){
					log_error(logger_entradasalida, "Se intenta leer por fuera del tamanio del archivo\n");
					goto error_io;
				}

				config = config_create(path);

				inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");

				char* leido = malloc(reg_tamanio);
				aux = bloques_dat + inicio_archivo*block_size + reg_puntero_archivo;

				memcpy((void*)leido, aux, reg_tamanio);

				printf("\nString a pasar a memoria: %s\n", leido);

				solicitar_almacen_memoria(pid, reg_direccion, leido, IO_FS_READ_FS);

				// devolver
				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				//config_destroy(config);

				break;

			case IO_FS_TRUNCATE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				pid = extraer_int_del_buffer(buffer);
				nombre = extraer_string_del_buffer(buffer);
				int nuevo_tamanio = extraer_int_del_buffer(buffer);

				log_info(logger_entradasalida, "PID: <%i> - Operacion: IO_FS_TRUNCATE", pid);
				usleep(tiempo_unidad_trabajo);
				log_info(logger_entradasalida,"PID: <%i> -  Truncar Archivo: <%s> -  Tamaño: <%i>",pid, nombre, nuevo_tamanio);	

				config = config_create(tomar_nombre_devolver_path(nombre));
				if(config == NULL){
					log_error(logger_entradasalida, "Error, no se encontro el archivo en el path.\n");
					goto finFs;
				}
				int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");
				 inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
				
                
				int cantidad_nueva_de_bloques = redondear_up(nuevo_tamanio, block_size);
				int cantidad_actual_de_bloques = redondear_up(tamanio_archivo, block_size);
				

				int diferencia = cantidad_actual_de_bloques - cantidad_nueva_de_bloques;

				if(diferencia == 0){
					printf("Archivo de mismo tamaño, no hace falta truncar");
					goto finFs;
				}
				
				if(cantidad_actual_de_bloques > cantidad_nueva_de_bloques){
					//si achicamos solo achicamos el archivo de metadata y marcamos los bloques restantes libres en el bitmap

					config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
					config_save(config);

					liberar_bloques_desde_hasta(inicio_archivo + cantidad_nueva_de_bloques, inicio_archivo + cantidad_actual_de_bloques);
					
				}else{
					// si queremos agrandar
					
					diferencia = abs(diferencia);

					if(buscar_lugar_bitmap(diferencia) == -2){ // -2 seria como un codigo de error, pq -1 ya lo usamos para otro
						log_error(logger_entradasalida, "No hay espacio suficiente en el FileSystem para truncar este archivo");
						goto finFs;
					}
					
					// verificar que haya espacio
					// chequeamos si entra
					int bloque_fin_archivo = inicio_archivo + redondear_up_con_cero(tamanio_archivo, block_size);
					int i = 0;

					while(i <= diferencia){
						if(bitarray_test_bit(bitmap, bloque_fin_archivo + i) == 1){
							log_info(logger_entradasalida, "PID: <%i> - Inicio Compactacion.", pid);
							compactar(nombre, config, nuevo_tamanio);
							log_info(logger_entradasalida, "PID: <%i> - Fin Compactacion.", pid);
							goto finFs;
						}
						i++;
					}
					

					if(i >= diferencia){
						
						config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
						//Seteo los Bloques como ocupados
						int j = 1;
						while (j <= diferencia){
							bitarray_set_bit(bitmap, bloque_fin_archivo + j);
							j++;
						}

						config_save(config);
					}
					
				}

					finFs:
				notificar_fin(fd_kernel, pid);
				msync(bitmap->bitarray, redondear_up(block_count, 8), MS_SYNC);
				
				//config_destroy(config);
				
				break;
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de E/S.\n");
				desconexion_kernel_entradasalida = 1;
				break;
			default: // La instruccion es incorrecta
				log_warning(logger_entradasalida, "La instruccion no es valida para esta interfaz de entrada/salida");
					error_io:
				
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

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
}

void solicitar_lectura_memoria(int pid, int direccion, int tamanio, op_code cod_op){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, pid);
	cargar_int_al_buffer(buffer, direccion);
	cargar_int_al_buffer(buffer, tamanio);

	t_paquete* paquete = crear_paquete(cod_op, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
}

void solicitar_almacen_memoria(int pid, int direccion, char* mensaje, op_code cod_op){ //MENSAJE NO DEBERIA SER UN VOID*??
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, pid);
	cargar_int_al_buffer(buffer, direccion);
	cargar_string_al_buffer(buffer, mensaje);

	t_paquete* paquete = crear_paquete(cod_op, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
}

int buscar_lugar_bitmap(int tamanio){
	int i = 0;
	int contador_libres = 0;
	int contador_libres_continuo = 0;
	
	
	while(i < block_count){
		if(bitarray_test_bit(bitmap, i) == 0){
			
			contador_libres ++;
			contador_libres_continuo ++;
			
			if(contador_libres_continuo >= tamanio){
				
				if(i == 0){
					return i;
				}
				
				return i - tamanio + 1;
			}
		}
		else{
			contador_libres_continuo = 0;
		}

		i ++;
	}
	
	if(contador_libres >= tamanio){

		return -1;
	}
	else{

		return -2;
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
		return;
	}

	int inicio_archivo = config_get_int_value(config, "BLOQUE_INICIAL");
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	for(int i = 0; i < redondear_up(tamanio_archivo, block_size); i++){
		bitarray_clean_bit(bitmap, inicio_archivo + i);
	}
}

void liberar_bloques_desde_hasta(int inicio, int fin){
	for(int i = 0; i <= fin - inicio; i++ ){
		bitarray_clean_bit(bitmap, inicio + i);
	}
}

void* eliminar_segun(char* nombre){
	for(int i = 0; i<list_size(archivos_metadata); i++){
		
		char* path = string_duplicate(list_get(archivos_metadata, i));
		if(strcmp(path, tomar_nombre_devolver_path(nombre)) == 0){
			return list_remove(archivos_metadata, i);
		}
	}
	return -1;
}

void compactar(char* nombre, t_config* config, int nuevo_tamanio){
	
	// ELIMINAMOS EL ARCHIVO A AGRANDAR, LO AGREGAMOS AL FINAL
	eliminar_segun(nombre);

	int bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
	
	int tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");
	
	// Actualizamos el archivo de metadata del archivo a truncar
	config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
	
	config_save(config);
	

	// Liberamos todos los bloques
	
	liberar_bloques_desde_hasta(0, block_count);

	// Creamos un buffer para guardar los datos del resto de los archivos
	void* buffer_auxiliar = calloc(block_size * block_count, 1);

	// Agregamos el archivo a truncar al final de la lista
	char* path_del_nombre = tomar_nombre_devolver_path(nombre);
	list_add(archivos_metadata, path_del_nombre);

	int bloques_desplazados = 0;

	for(int i = 0; i < list_size(archivos_metadata); i++){
		// Tomamos cada arhivo de la lista de archivos en el FS
		char* path_metadata = string_duplicate(list_get(archivos_metadata, i));

		// Abrimos el config de cada archivo en el FS
		t_config* config_actual = config_create(path_metadata);

		if(config_actual == NULL){
            log_info(logger_entradasalida, "El archivo de metada %s no existe", path_metadata);
            continue;
        }

		// Leemos el config actual
		int bloque_inicial_actual = config_get_int_value(config_actual, "BLOQUE_INICIAL");
		int tamanio_archivo_actual = config_get_int_value(config_actual, "TAMANIO_ARCHIVO"); 

		//printf("\n--ARCHIVO ACTUAL: %s\n--TAMANIO ARCHIVO: %i\n\n", path_metadata, tamanio_archivo_actual);

		// Copiamos datos del archivo actual
		if(strcmp(path_metadata, path_del_nombre) != 0){ // Verificamos que los datos a leer no pertenecen al archivo truncado en caso de que el nuevo tamanio se pase del espacio de memoria
			memcpy(buffer_auxiliar + bloques_desplazados * block_size, bloques_dat + bloque_inicial_actual * block_size, tamanio_archivo_actual);
		}else{
			memcpy(buffer_auxiliar + bloques_desplazados * block_size, bloques_dat + bloque_inicial_actual * block_size, tamanio_archivo);
		}
		
		char* desplazamiento_actual = string_itoa(bloques_desplazados);
        config_set_value(config_actual, "BLOQUE_INICIAL", desplazamiento_actual);
		config_save(config_actual);

		config_destroy(config_actual);

		bloques_desplazados += redondear_up(tamanio_archivo_actual, block_size);
	}
	for(int i = 0; i < bloques_desplazados; i++){
		bitarray_set_bit(bitmap, i); // bitarray_set_bit(bitmap, i);
	}
	
	// Sincronizamos archivo de bloques
	memcpy(bloques_dat, buffer_auxiliar, block_count * block_size);
	msync(bloques_dat, block_size*block_count, MS_SYNC);

	usleep(retraso_compactacion);
}
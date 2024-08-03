#include "memoria_de_instrucciones.h"

void guardar_instrucciones_en_memoria(int pid, char* path){
    t_instrucciones_por_proceso* instrucciones_proceso = malloc(sizeof(t_instrucciones_por_proceso));
    char* path_final = string_duplicate(path_instrucciones);

    string_append(&path_final, path);
    string_trim(&path_final);
    
    FILE* archivo = fopen(path_final, "rt");

    if(archivo == NULL){
        log_error(logger_memoria, "No se pudo abrir el archivo %s", path_final);
        return;
    }

    instrucciones_proceso->pid = malloc(sizeof(int)); //creo que esta de mas
    instrucciones_proceso->pid = pid;
    instrucciones_proceso->lista_de_instrucciones = list_create(); // Crea una lista ej => ["MOVE AX BX", "ADD DX CX"]

    char linea[100]; 
    
    while(fgets(linea, sizeof(linea), archivo) != NULL){
        linea[strcspn(linea, "\n")] = '\0'; // Eliminar el salto de línea al final de la línea leída
        char* linea_copia = strdup(linea); // Crear una copia de la línea leída para almacenarla en la lista
        printf("Procesada linea \" %s \" \n", linea_copia);
        char* linea_a_agregar = malloc(sizeof(linea_copia)); // HACER FREE AL SACAR EL PROCESO DE MEMORIA
        list_add(instrucciones_proceso->lista_de_instrucciones, linea_copia); // Agrega las intrucciones a la lista
    }

    fclose(archivo);
    
    printf("Size de instrucciones_proceso: %i\n", list_size(lista_de_instrucciones_por_proceso));
    // PONER HILO MUTEX PARA LISTA_dE_instrucciones_proceso
    list_add(lista_de_instrucciones_por_proceso, instrucciones_proceso ); 
}

void enviar_instruccion_a_cpu(int pid, int program_counter, int socket){

	if(list_size(lista_de_instrucciones_por_proceso) < 1){
		printf("No se cargaron pcbs en memoria: \n");
		return 0;
	}

	for(int i = 0; i <= list_size(lista_de_instrucciones_por_proceso); i++){

		t_instrucciones_por_proceso *instrucciones_proceso = list_get(lista_de_instrucciones_por_proceso, i);

		if(instrucciones_proceso->pid == pid){
			char *instruccion = list_get(instrucciones_proceso->lista_de_instrucciones, program_counter);

			if(strlen(instruccion) != 0){
				enviar_instruccion(instruccion, socket);
				log_info(logger_memoria, "INSTRUCCION ENVIADA A CPU: %s, PROGRAM_COUNTER: %d\n", instruccion, program_counter);
			}
			else{
				log_error(logger_memoria, "No hay instruccion para el pcb: %d\n", instrucciones_proceso->pid);
			}

			return;
		}
	}
    
	log_info(logger_memoria, "Error, no hay pid cargado en memoria\n");
}

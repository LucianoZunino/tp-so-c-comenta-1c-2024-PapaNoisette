#include "helpers.h"

void iniciar_logger_memoria(){
    logger_memoria = iniciar_logger("memoria.log", "Memoria");
}

void imprimir_config_memoria(){
    printf("\n============================================================\n");
    printf("Puerto de escucha: %s\n", puerto_escucha);
    printf("Tamaño de la memoria RAM: %d\n", tam_memoria);
    printf("Tamaño de página: %d\n", tam_pagina);
    printf("Path de instrucciones: %s\n", path_instrucciones);
    printf("Retardo de respuesta: %d\n", retardo_respuesta);
    printf("Cantidad de marcos (Frames): %d\n", cantidad_de_marcos);
    printf("============================================================\n\n");
}

/// @brief Imprime en un .txt la Memoria RAM
/// @note Muesta que tiene almacenado en la RAM en Hexadeciaml divididos por Frame
//        (Se le pasa la ruta en donde se guardara el .txt)
/// @param path
void print_memoria_RAM(char *path_log){
    FILE *archivo = fopen(path_log, "w");
    int numero_frame = 0;

    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return;
    }

    fprintf(archivo, "Contenido de la memoria RAM:\n\n");
    fprintf(archivo, "------------------ Frame %d -------------------\n", numero_frame);

    for(int i = 0; i < tam_memoria; i++){
        fprintf(archivo, "%02X ", ((unsigned char *)memoria_RAM)[i]);
        if((i + 1) % 16 == 0){
            fprintf(archivo, "\n");
        }
        if((i + 1) % 32 == 0 && (i + 1) != tam_memoria){
            numero_frame++;
            fprintf(archivo, "------------------ Frame %d -------------------\n", numero_frame);
        }
    }

    fclose(archivo);
}

/// @brief Imprime en un .txt el bitmap de Frames
/// @note Muesta la cantidad de Frames que hay en la memoria RAM, cual esta ocupado
//        y por cual proceso. (Se le pasa la ruta en donde se guardara el .txt)
/// @param path
void print_lista_de_frames(char *path_log){
    FILE *archivo = fopen(path_log, "w");

    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }

    fprintf(archivo, "Print lista de frames (%d frames)\n", cantidad_de_marcos);
    int num_frame = 0;
    int _closure_print_frame(Frame * frame){
        fprintf(archivo, "Frame %d ,Pid %d, Ocupado %d\n", num_frame, frame->pid, frame->ocupado);
        num_frame++;
    }
    list_iterate(lista_de_frames, _closure_print_frame);

    // Cierra el archivo
    fclose(archivo);
}

/// @brief Imprime en un .txt la lista de Procesos
/// @note Muesta la lista de Procesos que están creados y que páginas tienen asisgnadas
//        (Se le pasa la ruta en donde se guardara el .txt)
/// @param path
void print_lista_procesos(char *path_log){
    FILE *archivo = fopen(path_log, "w");
    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }
    int cant_procesos = list_size(lista_procesos);
    fprintf(archivo, "Print lista_procesos (%d procesos)\n\n", cant_procesos);

    for(int i = 0; i < cant_procesos; i++){ // itero todos los procesos
        Proceso *proceso = list_get(lista_procesos, i);
        fprintf(archivo, "{Proceso: %d, cantidad de paginas: %d}\n", proceso->pid, list_size(proceso->tabla_paginas));

        for(int j = 0; j < list_size(proceso->tabla_paginas); j++){
            int frame = list_get(proceso->tabla_paginas, j); 

            fprintf(archivo, "\tPagina %d -> Frame %d\n", j, frame);
        }
    }

    fclose(archivo);
}
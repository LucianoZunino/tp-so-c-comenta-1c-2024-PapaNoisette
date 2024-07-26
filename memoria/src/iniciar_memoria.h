#ifndef INICIAR_MEMORIA_H
#define INICIAR_MEMORIA_H

#include "main.h"
#include <utils/utils.h>

typedef struct{
    int numero_de_marco; // Número de marco de la página en la memoria física
} Pagina;

typedef struct {
    int pid;
    t_list * tabla_paginas; // list del tipo  PAGINA
} Proceso;

typedef struct {
    int ocupado; // 0 si está libre, 1 si está ocupado
    int pid; // PID del proceso que está utilizando el frame
} Frame;

void iniciar_memoria();
void iniciar_config_memoria();
void inicializar_estructuras_memoria();
void memoria_crear_proceso(int pid);
int resize_tamano_proceso( int pid, int nuevo_tamano);
int contar_frames_libres();
int asignar_y_marcar_frame_ocupado(int pid);
void liberar_frame(int numero_de_marco);
Proceso* buscar_proceso(t_list* lista, int pid);
int obtener_marco(int pid, int numero_pagina);
void finalizar_memoria();
int recibir_path_kernel(int socket);
void esperar_clientes();

void iniciar_logger_memoria();

void imprimir_config_memoria();

void print_memoria_RAM(char *path_log);

void print_lista_de_frames(char *path_log);

void print_lista_procesos(char *path_log);
#endif
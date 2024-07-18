#ifndef INICIAR_MEMORIA_H
#define INICIAR_MEMORIA_H

#include "main.h"
#include <utils/utils.h>

typedef struct{
    int numero_de_marco; // Número de marco de la página en la memoria física
    int bit_presencia; // Bit de validez, 1 si la página está en memoria, 0 si no
} Pagina;

typedef struct{
    Pagina *pagina;       // Puntero a un array de estructuras Pagina
    int numero_paginas;   // Número de páginas
} TablaDePaginas;

typedef struct {
    int pid;
    TablaDePaginas *tabla_paginas;
} TablaDePaginasPorProceso;

typedef struct {
    int ocupado; // 0 si está libre, 1 si está ocupado
    int pid; // PID del proceso que está utilizando el frame
} Frame;

void iniciar_memoria();
void iniciar_logger_memoria();
void iniciar_config_memoria();
void inicializar_memoria();
void inicializar_tabla_paginas(TablaDePaginas *tabla, int numero_paginas);
TablaDePaginasPorProceso *memoria_crear_proceso(int pid, int numero_paginas);
int resize_tamano_proceso(TablaDePaginasPorProceso *tablaDePaginasDelProceso, int nuevo_tamano);
void asignar_marco_a_tabla(TablaDePaginas *tabla, int numero_pagina, int numero_marco);
int contar_frames_libres();
int asignar_y_marcar_frame_ocupado(int pid);
void liberar_frame(int numero_de_marco);
void imprimir_config_memoria();
TablaDePaginasPorProceso* buscar_tabla_por_pid(t_list* lista, int pid);
void finalizar_memoria();
int recibir_path_kernel(int socket);
void guardar_instrucciones_en_memoria(t_pcb* pcb,char* path);
int asignar_frame(int);//nacho: esta funcion falta definirla o no se si te referis a asignar_y_marcar_frame_ocupado
void esperar_clientes();
#endif
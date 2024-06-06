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

void iniciar_memoria();
void iniciar_logger_memoria();
void iniciar_config_memoria();
void inicializar_tabla_paginas(TablaDePaginas *tabla, int numero_paginas);
void *asignar_memoria(size_t size);
void imprimir_config_memoria();
void finalizar_memoria();
#endif
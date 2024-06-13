#ifndef MAIN_MEMORIA_H
#define MAIN_MEMORIA_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_memoria.h"
#include "escuchar_cpu_memoria.h"
#include "escuchar_entradasalida_memoria.h"
#include "escuchar_kernel_memoria.h"
#include <utils/mensajes.h>

extern t_log* logger_memoria;

extern char* puerto_escucha;
extern int tam_memoria;
extern int tam_pagina;
extern int cantidad_de_marcos;
extern char* path_instrucciones;
extern int retardo_respuesta;

extern int fd_memoria;
extern int fd_cpu;
extern int fd_kernel;
extern int fd_entradasalida;

typedef struct{

    int pid;
    t_list *lista_de_instrucciones;

} t_miniPcb;

extern t_list *lista_de_miniPcb;
extern t_list *lista_de_tablas_de_paginas_por_proceso;

#endif
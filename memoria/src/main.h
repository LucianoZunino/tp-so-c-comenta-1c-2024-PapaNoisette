#ifndef MAIN_MEMORIA_H
#define MAIN_MEMORIA_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_memoria.h"

t_log* logger_memoria;

int puerto_escucha;
int tam_memoria;
int tam_pagina;
char* path_instrucciones;
int retardo_respuesta;

#endif
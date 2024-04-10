#ifndef MAIN_ENTRADASALIDA_H
#define MAIN_ENTRADASALIDA_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_entradasalida.h"

t_log* logger_entradasalida;


char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
int puerto_kernel;
char* ip_memoria;
int puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;


#endif
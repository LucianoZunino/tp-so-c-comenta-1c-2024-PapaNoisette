#ifndef MAIN_ENTRADASALIDA_H
#define MAIN_ENTRADASALIDA_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_entradasalida.h"

extern t_log* logger_entradasalida;


extern char* tipo_interfaz;
extern int tiempo_unidad_trabajo;
extern char* ip_kernel;
extern int puerto_kernel;
extern char* ip_memoria;
extern int puerto_memoria;
extern char* path_base_dialfs;
extern int block_size;
extern int block_count;


#endif
#ifndef MAIN_ENTRADASALIDA_H
#define MAIN_ENTRADASALIDA_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "utils/hello.h"
#include <utils/utils.h>
#include <semaphore.h>
#include <commons/bitarray.h>
#include "iniciar_entradasalida.h"
#include "escuchar_kernel_entradasalida.h"
#include "escuchar_memoria_entradasalida.h"

extern t_log* logger_entradasalida;

extern int tipo_de_interfaz;
extern int tiempo_unidad_trabajo;
extern char* ip_kernel;
extern char* puerto_kernel;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* path_base_dialfs;
extern int block_size;
extern int block_count;
extern int retraso_compactacion;

extern int fd_memoria;
extern int fd_kernel;

extern void* datos;

extern sem_t sem_stdout;
extern sem_t sem_fs_write;

extern int fd_bloques;
extern int fd_bitmap;
extern void* bloques_dat;
extern t_bitarray* bitmap;


#endif
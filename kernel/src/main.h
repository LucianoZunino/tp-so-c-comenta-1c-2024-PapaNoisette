#ifndef MAIN_KERNEL_H
#define MAIN_KERNEL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_kernel.h"
#include "escuchar_cpu_kernel.h"
#include "escuchar_entradasalida_kernel.h"
#include "escuchar_memoria_kernel.h"
#include "consola_interactiva.h"

extern t_log* logger_kernel;

extern char* puerto_escucha;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* ip_cpu;
extern char* puerto_cpu_dispatch;
extern char* puerto_cpu_interrupt;
extern char* algoritmo_planificacion;
extern int quantum;
extern char** recursos;
extern char** instancias_recursos;
extern int grado_multiprogramacion;

extern int fd_kernel;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_memoria;
extern int fd_entradasalida;

#endif
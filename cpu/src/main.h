#ifndef MAIN_CPU_H
#define MAIN_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_cpu.h"
#include "escuchar_kernel_cpu.h"
#include "escuchar_memoria_cpu.h"

extern t_log* logger_cpu;

extern char* ip_memoria;
extern char * puerto_memoria;
extern char * puerto_escucha_dispatch;
extern char * puerto_escucha_interrupt;
extern int cantidad_entradas_tlb;
extern char* algoritmo_tlb;

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_memoria;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;

#endif
#ifndef MAIN_CPU_H
#define MAIN_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_cpu.h"

extern t_log* logger_cpu;

extern char* ip_memoria;
extern char * puerto_memoria;
extern char * puerto_escucha_dispatch;
extern char * puerto_escucha_interrupt;
extern int cantidad_entradas_tlb;
extern char* algoritmo_tlb;

#endif
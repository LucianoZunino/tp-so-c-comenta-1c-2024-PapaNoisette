#ifndef MAIN_CPU_H
#define MAIN_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "iniciar_cpu.h"

t_log* logger_cpu;

char* ip_memoria;
int puerto_memoria;
int puerto_escucha_dispatch;
int puerto_escucha_interrupt;
int cantidad_entradas_tlb;
char* algoritmo_tlb;

#endif
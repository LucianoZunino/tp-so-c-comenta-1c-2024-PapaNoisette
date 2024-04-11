#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

t_log* logger_cpu;

char* ip_memoria;
int puerto_memoria;
int puerto_escucha_dispatch;
int puerto_escucha_interrupt;
int cantidad_entradas_tlb;
char* algoritmo_tlb;

int main(int argc, char* argv[]) {
    decir_hola("cpu");
    iniciar_cpu();
  	log_info(logger_cpu, "Arranca el modulo  CPUUUU");


    return 0;
}
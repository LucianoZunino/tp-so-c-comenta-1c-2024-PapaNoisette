#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

t_log* logger_kernel;

int puerto_escucha;
char* ip_memoria;
int puerto_memoria;
char* ip_cpu;
int puerto_cpu_dispatch;
int puerto_cpu_interrupt;
char* algoritmo_planificacion;
int quantum;
char** recursos;
char** instancias_recursos;
int grado_multiprogramacion;

int main(int argc, char* argv[]) {
    decir_hola("Kernel");
    iniciar_kernel();
    log_info(logger_kernel, "Arranca el modulo  KERNEL");

    return 0;
}

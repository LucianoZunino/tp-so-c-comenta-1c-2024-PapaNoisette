#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

t_log* logger_kernel;

char* puerto_escucha;
char* ip_memoria;
char* puerto_memoria;
char* ip_cpu;
char* puerto_cpu_dispatch;
char* puerto_cpu_interrupt;
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

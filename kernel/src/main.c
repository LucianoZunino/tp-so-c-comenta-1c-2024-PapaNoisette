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

    // Iniciar Kernel
    iniciar_kernel();
    log_info(logger_kernel, "Arranca el modulo  KERNEL");

    // Levantar Server Kernel (para que E/S se conecte)
    int fd_kernel = iniciar_servidor(puerto_escucha, logger_kernel, ">>>SERVER KERNEL INICIADO");

    // Conectarme como cliente a CPU-Dispatch (o debería ser a Interrupt?)
    int fd_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);


    // Conectarme como cliente a Memoria
    int fd_memoria = crear_conexion(ip_memoria, puerto_memoria);

    // Esperar conexion de E/S
    int fd_entradasalida = esperar_cliente(fd_kernel, logger_kernel, "E/S");

    // Finalizar Kernel
    finalizar_kernel();
    return 0;
}

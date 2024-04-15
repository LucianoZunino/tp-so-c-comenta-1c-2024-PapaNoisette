#include "main.h"

t_log* logger_memoria;

char* puerto_escucha;
int tam_memoria;
int tam_pagina;
char* path_instrucciones;
int retardo_respuesta;

int main(int argc, char* argv[]){
    decir_hola("Memoria");

    // Iniciar Memoria
    iniciar_memoria();
  	log_info(logger_memoria, "Arranca el modulo MEMORIA");

    // Levantar Server Memoria
   	int fd_memoria = iniciar_servidor(puerto_escucha, logger_memoria, ">>> Server Memoria escuchando... <<<");

    // Esperar conexion de CPU
    log_info(logger_memoria, "Esperando conexión de CPU");
    int fd_cpu = esperar_cliente(fd_memoria, logger_memoria, "CPU");

    // Esperar conexion de KERNEL
    log_info(logger_memoria, "Esperando conexión de Kernel");
    int fd_kernel = esperar_cliente(fd_memoria, logger_memoria, "KERNEL");

    // Esperar conexion de E/S
    log_info(logger_memoria, "Esperando conexión de E/S");
    int fd_entradasalida = esperar_cliente(fd_memoria, logger_memoria, "E/S");

    // Finalizar Memoria
   	finalizar_memoria();

    return 0;
}

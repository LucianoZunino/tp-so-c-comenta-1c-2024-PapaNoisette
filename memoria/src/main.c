#include "main.h"

t_log* logger_memoria;

char* puerto_escucha;
int tam_memoria;
int tam_pagina;
char* path_instrucciones;
int retardo_respuesta;

int main(int argc, char* argv[]) {
    decir_hola("Memoria");

    // Iniciar Memoria
    iniciar_memoria();
  	log_info(logger_memoria, "Arranca el modulo  MEMORIA");

    // Levantar Server Memoria
   	int fd_memoria = iniciar_servidor(puerto_escucha, logger_memoria, ">>>SERVER MEMORIA INICIADO");

	// Esperar conexion de CPU
	// Esperar conexion de E/S

	// Esperar conexion de KERNEL
    int fd_kernel = esperar_cliente(fd_memoria, logger_memoria, "KERNEL");

    // Finalizar Memoria
   	finalizar_memoria();
    return 0;
}

#include "main.h"

t_log* logger_memoria;

int puerto_escucha;
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
   // int fd_memoria = iniciar_servidor(puerto_escucha, logger_memoria, "Server de Memoria Iniciado");

    // Finalizar Memoria
   finalizar_memoria();
    return 0;
}

#include "main.h"

int main(int argc, char* argv[]) {
    decir_hola("Memoria");

    // Iniciar Memoria
    iniciar_memoria();

    // Levantar Server Memoria
   // int fd_memoria = iniciar_servidor(puerto_escucha, logger_memoria, "Server de Memoria Iniciado");

    // Finalizar Memoria
   finalizar_memoria();
    return 0;
}

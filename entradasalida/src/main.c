#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

int main(int argc, char* argv[]) {
    decir_hola("una Interfaz de Entrada/Salida");
    iniciar_entradasalida();
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    return 0;
}

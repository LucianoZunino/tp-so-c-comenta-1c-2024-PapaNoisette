#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

t_log* logger_entradasalida;


char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
int puerto_kernel;
char* ip_memoria;
int puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;

int main(int argc, char* argv[]) {
    decir_hola("una Interfaz de Entrada/Salida");
    iniciar_entradasalida();
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    return 0;
}

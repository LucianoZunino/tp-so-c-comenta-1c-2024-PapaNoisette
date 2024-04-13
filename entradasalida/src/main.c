
#include "main.h"
t_log* logger_entradasalida;

char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
char* puerto_kernel;
char* ip_memoria;
char* puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;

int main(int argc, char* argv[]) {
    decir_hola("una Interfaz de Entrada/Salida");
    iniciar_entradasalida();
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    return 0;
}

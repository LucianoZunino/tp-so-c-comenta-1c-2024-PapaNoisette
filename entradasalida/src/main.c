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

int main(int argc, char* argv[]){
    decir_hola("una Interfaz de Entrada/Salida");

    // Iniciar interfaz I/O
    iniciar_entradasalida();
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    // Se conecta como cliente a MEMORIA
    int fd_memoria = crear_conexion(ip_memoria, puerto_memoria,logger_entradasalida);

    // Se conecta como cliente a KERNEL
    int fd_kernel = crear_conexion(ip_kernel, puerto_kernel,logger_entradasalida);

    // Finalizar Interfaz E/S
    finalizar_entradasalida();

    return 0;
}

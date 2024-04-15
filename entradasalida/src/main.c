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

int fd_memoria;
int fd_kernel;

int main(int argc, char* argv[]){
    decir_hola("una Interfaz de Entrada/Salida");

    // Iniciar interfaz I/O
    iniciar_entradasalida();
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    // Se conecta como cliente a MEMORIA
    fd_memoria = crear_conexion(ip_memoria, puerto_memoria);

    // Se conecta como cliente a KERNEL
    fd_kernel = crear_conexion(ip_kernel, puerto_kernel);

    // Escucha los mensajes Memoria-E/S
    pthread_t hilo_memoria_entradasalida;
	pthread_create(&hilo_memoria_entradasalida, NULL, (void*)escuchar_mensajes_memoria_entradasalida, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_memoria_entradasalida); // Hace que el hilo se desacople del principal y se ejecute en paralelo

    // Escucha los mensajes Kernel-E/S
    pthread_t hilo_kernel_entradasalida;
	pthread_create(&hilo_kernel_entradasalida, NULL, (void*)escuchar_mensajes_kernel_entradasalida, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_kernel_entradasalida, NULL); // Frena el hilo principal hasta que el hilo_kernel_entradasalida no finalice
	// Porque si se el hilo_kernel_entradasalida se desacopla del principal termina el modulo Entradasalida

    // Finalizar Interfaz E/S
    finalizar_entradasalida();

    return 0;
}

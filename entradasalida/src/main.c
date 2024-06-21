#include "main.h"

t_log* logger_entradasalida;

int tipo_de_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
char* puerto_kernel;
char* ip_memoria;
char* puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;
int retraso_compactacion;

int fd_memoria;
int fd_kernel;

int main(int argc, char** argv[]){
    //if(argc < 1 ){log_error(logger_entradasalida, "Debe pasar el archivo de configuracion "); abort();}
    decir_hola("una Interfaz de Entrada/Salida");

    // Iniciar interfaz I/O
    iniciar_entradasalida(); //argv[1]
    log_info(logger_entradasalida, "Arranca el modulo  Entrada/Salida");

    // Se conecta como cliente a MEMORIA
    fd_memoria = crear_conexion(ip_memoria, puerto_memoria, logger_entradasalida);

    // Se conecta como cliente a KERNEL
    fd_kernel = crear_conexion(ip_kernel, puerto_kernel, logger_entradasalida);

    if (realizar_handshake(logger_entradasalida, fd_kernel, HANDSHAKE_ENTRADASALIDA) == -1){
        return EXIT_FAILURE;
    }

    if (realizar_handshake(logger_entradasalida, fd_memoria, HANDSHAKE_ENTRADASALIDA) == -1){
        return EXIT_FAILURE;
    }

    // Escucha los mensajes Memoria-E/S
    pthread_t hilo_memoria_entradasalida;
	pthread_create(&hilo_memoria_entradasalida, NULL, (void*)escuchar_mensajes_memoria_entradasalida, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_memoria_entradasalida); // Hace que el hilo se desacople del principal y se ejecute en paralelo

    // Escucha los mensajes Kernel-E/S
    pthread_t hilo_kernel_entradasalida;
    switch(tipo_de_interfaz){
        case GENERICA:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_generica, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;

        case STDIN:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_stdin, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
        case STDOUT:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_stdout, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
        case DIAL_FS:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_dialfs, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
    }
	// Frena el hilo principal hasta que el hilo_kernel_entradasalida no finalice
	// Porque si se el hilo_kernel_entradasalida se desacopla del principal termina el modulo Entradasalida

    // Finalizar Interfaz E/S
    finalizar_entradasalida();

    return 0;
}

#include "main.h"

t_log* logger_memoria;

char* puerto_escucha;
int tam_memoria;
int tam_pagina;
int numero_paginas;
char* path_instrucciones;
int retardo_respuesta;

int fd_memoria;
int fd_cpu;
int fd_kernel;
int fd_entradasalida;

int main(int argc, char* argv[]){
    decir_hola("Memoria");
    // Iniciar Memoria
    iniciar_memoria();
    printf("Memoria iniciada, lista de mini PCB size: %i\n", list_size(lista_de_miniPcb));
  	log_info(logger_memoria, "Arranca el modulo MEMORIA");
    printf("path_instrucciones%s\n",path_instrucciones);

    // Levantar Server Memoria
   	fd_memoria = iniciar_servidor(puerto_escucha, logger_memoria, ">>> Server Memoria escuchando... <<<");

    // Esperar conexion de CPU
    log_info(logger_memoria, "Esperando conexión de CPU");
    fd_cpu = esperar_cliente(fd_memoria, logger_memoria, "CPU");

    // Esperar conexion de KERNEL
    log_info(logger_memoria, "Esperando conexión de Kernel");
    fd_kernel = esperar_cliente(fd_memoria, logger_memoria, "KERNEL");

/* Tenemos que escuchar más de un cliente
    // Esperar conexion de E/S
    log_info(logger_memoria, "Esperando conexión de E/S");
    fd_entradasalida = esperar_cliente(fd_memoria, logger_memoria, "E/S");
*/

    log_info(logger_memoria, "Esperando conexion de Interfaz E/S");
    pthread_t hilo_escucha_io;
    if (pthread_create(&hilo_escucha_io, NULL, (void *)(esperar_clientes), NULL) == -1){
        log_error(logger_memoria, "No se pudo crear el hilo de escucha E/S.");
        return EXIT_FAILURE;
    }
    pthread_detach(hilo_escucha_io);
  

    // Escucha los mensajes Kernel-Memoria
    pthread_t hilo_kernel_memoria;
	pthread_create(&hilo_kernel_memoria, NULL, (void*)escuchar_mensajes_kernel_memoria, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_kernel_memoria); // Hace que el hilo se desacople del principal y se ejecute en paralelo
  
  // Escucha los mensajes CPU-Memoria
    pthread_t hilo_cpu_memoria;
	pthread_create(&hilo_cpu_memoria, NULL, (void*)escuchar_mensajes_cpu_memoria, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	//pthread_join(hilo_cpu_memoria,NULL); // Hace que el hilo se desacople del principal y se ejecute en paralelo
  pthread_detach(hilo_cpu_memoria); // Hace que el hilo se desacople del principal y se ejecute en paralelo

/*
    // Escucha los mensajes E/S-Memoria
    pthread_t hilo_entradasalida_memoria;
	pthread_create(&hilo_entradasalida_memoria, NULL, (void*)escuchar_mensajes_entradasalida_memoria, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_entradasalida_memoria, NULL); // Frena el hilo principal hasta que el hilo_entradasalida_memoria no finalice
	// Porque si se el hilo_entradasalida_memoria se desacopla del principal termina el modulo Memoria
*/
    // Finalizar Memoria
   	finalizar_memoria();

    return 0;
}


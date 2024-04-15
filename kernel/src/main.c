#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

t_log* logger_kernel;

char* puerto_escucha;
char* ip_memoria;
char* puerto_memoria;
char* ip_cpu;
char* puerto_cpu_dispatch;
char* puerto_cpu_interrupt;
char* algoritmo_planificacion;
int quantum;
char** recursos;
char** instancias_recursos;
int grado_multiprogramacion;

int fd_kernel;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;
int fd_entradasalida;

int main(int argc, char* argv[]){
    decir_hola("Kernel");

    // Iniciar Kernel
    iniciar_kernel();
    log_info(logger_kernel, "Arranca el modulo KERNEL");

    // Levantar Server Kernel (para que E/S se conecte)
    fd_kernel = iniciar_servidor(puerto_escucha, logger_kernel, ">>> Server Kernel escuchando... <<<");

    // Se conecta como cliente a CPU-Dispatch
    fd_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);

    // Se conecta como cliente a CPU-Interrupt
    fd_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);

    // Se conecta como cliente a MEMORIA
    fd_memoria = crear_conexion(ip_memoria, puerto_memoria);

    // Esperar conexion de E/S
    log_info(logger_kernel, "Esperando conexion de Interfaz E/S");
    fd_entradasalida = esperar_cliente(fd_kernel, logger_kernel, "E/S");

    // Escuchar los mensajes de Dispatch-Kernel
    pthread_t hilo_dispatch_kernel;
	pthread_create(&hilo_dispatch_kernel, NULL, (void*)escuchar_mensajes_dispatch_kernel, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_dispatch_kernel); // Hace que el hilo se desacople del principal y se ejecute en paralelo

    // Escuchar los mensajes de Interrupt-Kernel
    pthread_t hilo_interrupt_kernel;
	pthread_create(&hilo_interrupt_kernel, NULL, (void*)escuchar_mensajes_interrupt_kernel, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_interrupt_kernel); // Hace que el hilo se desacople del principal y se ejecute en paralelo

    // Escuchar los mensajes de Memoria-Kernel
    pthread_t hilo_memoria_kernel;
	pthread_create(&hilo_memoria_kernel, NULL, (void*)escuchar_mensajes_memoria_kernel, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_memoria_kernel); // Hace que el hilo se desacople del principal y se ejecute en paralelo

    // Escuchar los mensajes de E/S-Kernel
    pthread_t hilo_entradasalida_kernel;
	pthread_create(&hilo_entradasalida_kernel, NULL, (void*)escuchar_mensajes_entradasalida_kernel, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_entradasalida_kernel, NULL); // Frena el hilo principal hasta que el hilo_entradasalida_kernel no finalice
	// Porque si se el hilo_entradasalida_kernel se desacopla del principal termina el modulo Kernel

    // Finalizar Kernel
    finalizar_kernel();
    
    return 0;
}

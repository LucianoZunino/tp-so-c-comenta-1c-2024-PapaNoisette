#include <stdlib.h>
#include <stdio.h>
//#include <utils/utils.h>
#include "main.h"

//Variables de la config
t_log* logger_cpu;
char* ip_memoria;
char * puerto_memoria;
char* puerto_escucha_dispatch;
char * puerto_escucha_interrupt;
int cantidad_entradas_tlb;
char* algoritmo_tlb;
int tamanio_pagina=-1;
//Variables de socket
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;
int fd_kernel_dispatch;
int fd_kernel_interrupt;

uint32_t PROGRAM_COUNTER=0;

t_registros_cpu * registros_cpu;

int main(int argc, char* argv[]){
	iniciar_cpu();
	

  	//log_info(logger_cpu, "Arranca el modulo CPU");
    // Inicia el server de cpu dispatch
	fd_cpu_dispatch = iniciar_servidor(puerto_escucha_dispatch, logger_cpu, ">>> Server CPU-Dispatch iniciado <<<");
 
	// Iniciar el server de cpu interrupt 
	fd_cpu_interrupt = iniciar_servidor(puerto_escucha_interrupt, logger_cpu, ">>> Server CPU-Interrupt iniciado <<<");

	fd_memoria = crear_conexion(ip_memoria, puerto_memoria, logger_cpu);
	log_info(logger_cpu, ">>> CPU conectado al servidor de Memoria <<<");
	//Handshake CPU - Memoria
	if (realizar_handshake(logger_cpu, fd_memoria, HANDSHAKE_CPU) == -1){
        return EXIT_FAILURE;
    }
	//tamanio_pagina=consultar_tamanio_pagina_memoria();//es un dato necesario en cpu y no viene en config
  	// Esperar al cliente Kernel en dispatch
    log_info(logger_cpu, "Esperando conexion de Kernel en Dispatch");
  	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, logger_cpu, "Kernel-Dispatch");

  	// Esperar al cliente Kernel en interrupt
	log_info(logger_cpu, "Esperando conexion de Kernel en Interrupt");
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, logger_cpu, "Kernel-Interrupt");


	
	// Escuchar los mensajes de Kernel-Interrupt
	pthread_t hilo_kernel_interrupt;
	pthread_create(&hilo_kernel_interrupt, NULL, (void*)escuchar_mensajes_kernel_interrupt, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_kernel_interrupt); // Hace que el hilo se desacople del principal y se ejecute en paralelo

// Escuchar los mensajes de Kernel-Dispatch
	pthread_t hilo_kernel_dispatch;
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)escuchar_mensajes_kernel_dispatch, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_kernel_dispatch,NULL); // Hace que el hilo se desacople del principal y se ejecute en paralelo

	

	// Finalizar CPU
	finalizar_cpu();
	
return 0;
}



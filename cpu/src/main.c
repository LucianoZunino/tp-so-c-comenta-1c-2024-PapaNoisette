#include <stdlib.h>
#include <stdio.h>
//#include <utils/hello.h>
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

//Variables de socket
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;
int fd_kernel_dispatch;
int fd_kernel_interrupt;

//Variables de los registros de la CPU
uint32_t PROGRAM_COUNTER=0;
uint8_t AX;
uint8_t BX;
uint8_t CX;
uint8_t DX;
uint32_t EAX;
uint32_t EBX;
uint32_t ECX;
uint32_t EDX;
uint32_t SI;
uint32_t DI;

extern t_list* INSTRUCTION_LIST;


int main(int argc, char* argv[]){
    decir_hola("CPU");

	// Iniciar CPU
    iniciar_cpu();
  	log_info(logger_cpu, "Arranca el modulo CPU");

    // Inicia el server de cpu dispatch
	fd_cpu_dispatch = iniciar_servidor(puerto_escucha_dispatch, logger_cpu, ">>> Server CPU-Dispatch escuchando... <<<");
 
	// Iniciar el server de cpu interrupt 
	fd_cpu_interrupt = iniciar_servidor(puerto_escucha_interrupt, logger_cpu, ">>> Server CPU-Interrupt escuchando... <<<");

	fd_memoria = crear_conexion(ip_memoria, puerto_memoria, logger_cpu);
	log_info(logger_cpu,">>>CPU se conecta al server MEMORIA");

  	// Esperar al cliente Kernel en dispatch
    log_info(logger_cpu, "Esperando conexion de Kernel en Dispatch");
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, logger_cpu, "Kernel-Dispatch");

  	// Esperar al cliente Kernel en interrupt
	log_info(logger_cpu, "Esperando conexion de Kernel en Interrupt");
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, logger_cpu, "Kernel-Interrupt");

	// Escuchar los mensajes de Kernel-Dispatch
	pthread_t hilo_kernel_dispatch;
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)escuchar_mensajes_kernel_dispatch, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_kernel_dispatch); // Hace que el hilo se desacople del principal y se ejecute en paralelo

	// Escuchar los mensajes de Kernel-Interrupt
	pthread_t hilo_kernel_interrupt;
	pthread_create(&hilo_kernel_interrupt, NULL, (void*)escuchar_mensajes_kernel_interrupt, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_detach(hilo_kernel_interrupt); // Hace que el hilo se desacople del principal y se ejecute en paralelo

	// Escuchar los mensajes de Memoria-CPU
	pthread_t hilo_memoria_cpu;
	pthread_create(&hilo_memoria_cpu, NULL, (void*)escuchar_mensajes_memoria_cpu, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_memoria_cpu, NULL); // Frena el hilo principal hasta que el hilo_memoria_cpu no finalice
	// Porque si se el hilo_memoria_cpu se desacopla del principal termina el modulo CPU

	// Finalizar CPU
	finalizar_cpu();
	
	return 0;
}
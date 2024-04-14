#include <stdlib.h>
#include <stdio.h>
//#include <utils/hello.h>
//#include <utils/utils.h>
#include "main.h"

t_log* logger_cpu;

char* ip_memoria;
char * puerto_memoria;
char* puerto_escucha_dispatch;
char * puerto_escucha_interrupt;
int  cantidad_entradas_tlb;
char* algoritmo_tlb;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;
int fd_kernel_dispatch;
int fd_kernel_interrupt;


int main(int argc, char* argv[]) {
    decir_hola("cpu");
    iniciar_cpu();
  	log_info(logger_cpu, "Arranca el modulo CPUUUU");
    //iniciar server de cpu dispatch
	fd_cpu_dispatch= iniciar_servidor(puerto_escucha_dispatch,logger_cpu,">>> SERVER CPU-DISPATCH INICIADO");
 
	// iniciar server de cpu interrupt 
	fd_cpu_interrupt= iniciar_servidor(puerto_escucha_interrupt,logger_cpu,">>> SERVER CPU-INTERRUPT INICIADO");

    //Esperar al cliente Kernel en dispatch
    log_info(logger_cpu, "Esperando conexion de Kernel en Dispatch");
    fd_kernel_dispatch=esperar_cliente(fd_cpu_dispatch,logger_cpu, "Kernel-Dispatch");

    //Esperar al cliente Kernel en interrupt
	fd_kernel_interrupt=esperar_cliente(fd_cpu_interrupt,logger_cpu, "Kernel-Interrupt");

	//conectar como CLIENTE a memoria
	fd_memoria = crear_conexion(ip_memoria, puerto_memoria);
	
	//log_info(logger_cpu,">>>CPU se conecta al server MEMORIA");
	log_info(logger_cpu,">>>>>FINALIZA CPU<<<<<");
	return 0;
}
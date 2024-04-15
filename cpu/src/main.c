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
  	log_info(logger_cpu, "Arranca el modulo  CPUUUU");
    //iniciar server de cpu dispatch
  fd_cpu_dispatch= iniciar_servidor(puerto_escucha_dispatch,logger_cpu,">>>SERVER CPU-DISPATCH INICIADO");
 
   // iniciar server de cpu interrupt 
  fd_cpu_interrupt= iniciar_servidor(puerto_escucha_interrupt,logger_cpu,">>>SERVER CPU-INTERRUPT INICIADO");


    //Esperar al cliente Kernel en dispatch
  fd_kernel_dispatch=esperar_cliente(fd_cpu_dispatch,logger_cpu,"Kernel-Dispatch");

    //Esperar al cliente Kernel en interrupt
    fd_kernel_interrupt=esperar_cliente(fd_cpu_interrupt,logger_cpu,"Kernel-Interrupt");

    //creo los hilos para atender los sockets
    //pthread_create(&hilo_conexion_kernel_dispatch,NULL,(void*) atender_kernel_dispatch,NULL);
    //pthread_create(&hilo_conexion_kernel_interrupt,NULL,(void*) atender_kernel_interrupt,NULL);

	//Mientras las conexiones a kernel y memoria esten activas, el cpu sigue ejecutando
	//pthread_join(hilo_conexion_kernel,NULL);    ????
	// pthread_join(hilo_conexion_memoria,NULL);  ????
  	//pthread_join(hilo_conexion_kernel_dispatch,NULL);


  //conectar como CLIENTE a memoria
  fd_memoria =crear_conexion(ip_memoria,puerto_memoria,logger_cpu);
  log_info(logger_cpu,">>>CPU se conecta al server MEMORIA");




 
 liberar_conexion(fd_cpu_dispatch);
 liberar_conexion(fd_cpu_interrupt);

 liberar_conexion(fd_kernel_dispatch);
 liberar_conexion(fd_kernel_interrupt);


log_info(logger_cpu,">>>>>FINALIZA CPU<<<<<");

finalizar_cpu();

  return 0;
}
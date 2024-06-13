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
int tamanio_pagina;
//Variables de socket
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;
int fd_kernel_dispatch;
int fd_kernel_interrupt;

uint32_t PROGRAM_COUNTER=0;

t_registros_cpu * registros_cpu;
t_list * tlb;

int main(int argc, char* argv[]){
	iniciar_cpu();
  	log_info(logger_cpu, "Arranca el modulo CPU");
	iniciar_cpu();
    registros_cpu = malloc(sizeof(t_registros_cpu));//pasasr a iniciar_cpu()
    // Inicia el server de cpu dispatch
	fd_cpu_dispatch = iniciar_servidor(puerto_escucha_dispatch, logger_cpu, ">>> Server CPU-Dispatch escuchando... <<<");
 
	// Iniciar el server de cpu interrupt 
	fd_cpu_interrupt = iniciar_servidor(puerto_escucha_interrupt, logger_cpu, ">>> Server CPU-Interrupt escuchando... <<<");

/* no  me conecto a memoria por ahora
	fd_memoria = crear_conexion(ip_memoria, puerto_memoria, logger_cpu);
	log_info(logger_cpu,">>>CPU se conecta al server MEMORIA");
// Handshake CPU - Memoria
	if (realizar_handshake(logger_cpu, fd_memoria, HANDSHAKE_CPU) == -1){
        return EXIT_FAILURE;
    }*/
  	// Esperar al cliente Kernel en dispatch
    log_info(logger_cpu, "Esperando conexion de Kernel en Dispatch");
  ////  fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, logger_cpu, "Kernel-Dispatch");

  	// Esperar al cliente Kernel en interrupt
	//log_info(logger_cpu, "Esperando conexion de Kernel en Interrupt");
//	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, logger_cpu, "Kernel-Interrupt");


	// Escuchar los mensajes de Kernel-Dispatch
	pthread_t hilo_kernel_dispatch;
	////pthread_create(&hilo_kernel_dispatch, NULL, (void*)escuchar_mensajes_kernel_dispatch, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	////pthread_detach(hilo_kernel_dispatch); // Hace que el hilo se desacople del principal y se ejecute en paralelo

	// Escuchar los mensajes de Kernel-Interrupt
	pthread_t hilo_kernel_interrupt;
//	pthread_create(&hilo_kernel_interrupt, NULL, (void*)escuchar_mensajes_kernel_interrupt, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	//pthread_detach(hilo_kernel_interrupt); // Hace que el hilo se desacople del principal y se ejecute en paralelo
/* 
	// Escuchar los mensajes de Memoria-CPU
	pthread_t hilo_memoria_cpu;
	pthread_create(&hilo_memoria_cpu, NULL, (void*)escuchar_mensajes_memoria_cpu, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	pthread_join(hilo_memoria_cpu, NULL); // Frena el hilo principal hasta que el hilo_memoria_cpu no finalice
	// Porque si se el hilo_memoria_cpu se desacopla del principal termina el modulo CPU
	
	


*/

	//while(1); //este while lo tengo que dejar o remplazar por un hilo de funcionamiento de cpu
	//si muere el hilo padre muere el escuchar que es el hilo hijo


	// Finalizar CPU
	//finalizar_cpu();
	
//pruebas mmu

tamanio_pagina=32;
int dir_log= 64;
algoritmo_tlb ="LRU";
printf("dir logica :%d num pagina: %d\n",dir_log,calcular_numero_de_pagina(dir_log));
printf("dir logica :%d desplazamiento: %d\n",dir_log,calcular_desplazamiento(dir_log,calcular_numero_de_pagina(dir_log)));
cantidad_entradas_tlb=0;
t_entrada_tlb *entrada1= malloc(sizeof(t_entrada_tlb));
t_entrada_tlb *entrada2= malloc(sizeof(t_entrada_tlb));
t_entrada_tlb *entrada3= malloc(sizeof(t_entrada_tlb));
t_entrada_tlb *entrada4= malloc(sizeof(t_entrada_tlb));
t_entrada_tlb *entrada5= malloc(sizeof(t_entrada_tlb));
t_entrada_tlb *entrada6= malloc(sizeof(t_entrada_tlb));

entrada1->marco=5,entrada1->pagina=1,entrada1->pid=1;
entrada2->marco=12,entrada2->pagina=5,entrada2->pid=2;
entrada3->marco=23,entrada3->pagina=2,entrada3->pid=3;
entrada4->marco=66,entrada4->pagina=7,entrada4->pid=4;
entrada5->marco=32,entrada5->pagina=8,entrada5->pid=5;
entrada6->marco=97,entrada6->pagina=11,entrada6->pid=7;
/*
agregar_entrada_tlb(entrada1);
agregar_entrada_tlb(entrada2);
agregar_entrada_tlb(entrada3);
agregar_entrada_tlb(entrada4);
agregar_entrada_tlb(entrada5);
mostrar_tlb();
agregar_entrada_tlb(entrada6);
mostrar_tlb();

mostrar_tlb();

buscar_marco_tlb(3,64);
mostrar_tlb();*/
buscar_marco_tlb(1,1);

return 0;
}
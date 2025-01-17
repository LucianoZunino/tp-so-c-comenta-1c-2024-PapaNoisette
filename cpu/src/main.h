#ifndef MAIN_CPU_H
#define MAIN_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/utils.h>
#include <utils/mensajes.h>
#include "ciclo_instrucciones.h"
#include "escuchar_kernel_cpu.h"
#include "escuchar_memoria_cpu.h"
#include "iniciar_cpu.h"
#include "mmu.h"


//Variables de la config
extern t_log* logger_cpu;
extern char* ip_memoria;
extern char * puerto_memoria;
extern char * puerto_escucha_dispatch;
extern char * puerto_escucha_interrupt;
extern int cantidad_entradas_tlb;
extern char* algoritmo_tlb;
extern int tamanio_pagina; //hay que consultarselo a memoria??
extern bool flag_desalojo;
extern bool flag_interrupt;

//Variables de socket
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_memoria;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;

//Variables de los registros de la CPU
extern uint32_t PROGRAM_COUNTER;
extern uint8_t AX;
extern uint8_t BX;
extern uint8_t CX;
extern uint8_t DX;
extern uint32_t EAX;
extern uint32_t EBX;
extern uint32_t ECX;
extern uint32_t EDX;
extern uint32_t SI;
extern uint32_t DI;

extern t_registros_cpu * registros_cpu;

extern t_pcb* EXEC;

extern  t_list * tlb;

extern sem_t sem_interrupt;
extern sem_t sem_desalojo;

#endif
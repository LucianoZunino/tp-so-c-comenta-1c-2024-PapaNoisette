#ifndef MAIN_KERNEL_H
#define MAIN_KERNEL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include <semaphore.h>
#include "iniciar_kernel.h"
#include "escuchar_cpu_kernel.h"
#include "escuchar_entradasalida_kernel.h"
#include "escuchar_memoria_kernel.h"
#include "consola_interactiva.h"
#include "acciones_proceso.h"
#include "planificador_largo_plazo.h"
#include <utils/mensajes.h>

extern t_log* logger_kernel;

extern char* puerto_escucha;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* ip_cpu;
extern char* puerto_cpu_dispatch;
extern char* puerto_cpu_interrupt;
extern char* algoritmo_planificacion;
extern int quantum;
extern char** recursos;
extern char** instancias_recursos;
extern int grado_multiprogramacion;

extern int fd_kernel;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_memoria;
extern int fd_entradasalida;

extern int grado_actual_multiprogramacion;

// HILO PARA EL NEXT_PID
extern uint32_t next_pid; //creo que falta inicializar
extern pthread_mutex_t mutex_next_pid;

// LISTAS DE ESTADOS, DEBEMOS USARLAS COMO FIFO
extern t_list *NEW;
extern t_list *READY;
extern t_list *BLOCKED;
extern t_pcb *RUNNING;
extern t_list *EXIT;

// HILOS MUTEX PARA AGREGAR CON MUTUA EXCLUSION PARA QUE NO HAYA SUPERPOSICION
// EN LOS PROCESOS QUE COMPARTAN DICHAS COLAS DE ESTADOS.
extern pthread_mutex_t mutex_NEW;
extern pthread_mutex_t mutex_READY;
extern pthread_mutex_t mutex_BLOCKED;
extern pthread_mutex_t mutex_RUNNING;
extern pthread_mutex_t mutex_EXIT;

// SE USA PARA EL CREAR_PROCESO
extern pthread_mutex_t socket_memoria_mutex;

#endif
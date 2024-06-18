#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <utils/utils.h>
#include "main.h"

typedef struct{
    char* nombre;
    int instancias;
    t_queue* cola_de_espera;
    t_list* pcb_asignados;
    pthread_mutex_t mutex;
}t_recurso;

extern t_list* recursos_disponibles;

void interrumpir_cpu(t_pcb *pcb, motivo_interrupcion motivo);
void pcb_destruir(t_pcb *pcb);
void enviar_proceso_cpu(t_pcb *pcb, int socket, op_code op_code);
int buscar_index_por_pid(t_list* lista, int pid);
void restar_instancia(char* recurso, t_pcb*);
void sumar_instancia(char* recurso, t_pcb* pcb);
int buscar_interfaz(char* nombre);

#endif
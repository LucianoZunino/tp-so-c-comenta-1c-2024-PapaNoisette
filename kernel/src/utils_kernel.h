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
extern char* estado_pcb_desc[6];

void interrumpir_cpu(t_pcb *pcb, motivo_interrupcion motivo);
void pcb_destruir(t_pcb *pcb);
void enviar_proceso_cpu(t_pcb *pcb, int socket, op_code op_code);
int buscar_index_por_pid(t_list* lista, int pid);
void restar_instancia(char* recurso, t_pcb*);
int sumar_instancia(char* recurso, t_pcb* pcb);
int buscar_interfaz(char* nombre);
bool lista_contiene_pcb(t_list* lista, t_pcb* pcb);
void cambio_de_estado(t_pcb* pcb, int estado);
void enviar_a_exit(t_pcb* pcb, char* motivo);
bool verificar_existencia_de_interfaz(int indice_de_interfaz, t_pcb* pcb);
void validar_desalojo();

#endif
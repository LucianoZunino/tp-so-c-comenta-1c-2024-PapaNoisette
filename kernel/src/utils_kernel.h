#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <utils/utils.h>
#include "main.h"

void interrumpir_cpu(t_pcb *pcb, motivo_interrupcion motivo);
void pcb_destruir(t_pcb *pcb);
void enviar_proceso_cpu(t_pcb *pcb, int socket, op_code op_code);
int buscar_index_por_pid(t_list* lista, int pid);

#endif
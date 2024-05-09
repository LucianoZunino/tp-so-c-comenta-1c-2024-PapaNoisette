#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <utils/utils.h>
#include "main.h"

void interrumpir_cpu(t_pcb *pcb, motivo_interrupcion motivo);
void pcb_destruir(t_pcb *pcb);

#endif
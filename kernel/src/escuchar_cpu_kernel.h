#ifndef ESCUCHAR_CPU_KERNEL_H
#define ESCUCHAR_CPU_KERNEL_H

#include "main.h"
#include "utils/mensajes.h"

void escuchar_mensajes_dispatch_kernel();
void escuchar_mensajes_interrupt_kernel();
void bloquear_proceso(t_pcb *pcb, char* motivo);

#endif
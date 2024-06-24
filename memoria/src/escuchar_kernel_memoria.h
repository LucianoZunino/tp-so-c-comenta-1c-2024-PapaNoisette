#ifndef ESCUCHAR_KERNEL_MEMORIA_H
#define ESCUCHAR_KERNEL_MEMORIA_H

#include "main.h"
#include "utils/mensajes.h"
#include <utils/mensajes.h>

void escuchar_mensajes_kernel_memoria();
void guardar_instrucciones_en_memoria(t_pcb* pcb,char* path);
//void obtener_mensaje(t_buffer* buffer);

#endif
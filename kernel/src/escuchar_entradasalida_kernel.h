#ifndef ESCUCHAR_ENTRADASALIDA_KERNEL_H
#define ESCUCHAR_ENTRADASALIDA_KERNEL_H

#include "main.h"

void escuchar_mensajes_entradasalida_kernel(int indice_interfaz);
void esperar_entradasalida(sem_t sem_ocupado, int indice);

#endif
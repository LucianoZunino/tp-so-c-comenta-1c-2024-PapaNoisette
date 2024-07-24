#ifndef ESCUCHAR_CPU_MEMORIA_H
#define ESCUCHAR_CPU_MEMORIA_H

#include "main.h"

void escuchar_mensajes_cpu_memoria();

void* ejecutar_mov_in(int tamanio, int dir_fisica, int pid);
void ejecutar_mov_out(int tamanio, int dir_fisica, int pid, void *datos);
bool validar_espacio_de_memoria(int pid, int dir_fisica, int tam, int* indice_de_marco);

#endif
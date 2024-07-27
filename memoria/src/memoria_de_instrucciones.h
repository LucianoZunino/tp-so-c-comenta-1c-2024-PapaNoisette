#ifndef MEMORIA_DE_INSTRUCCIONES_H
#define MEMORIA_DE_INSTRUCCIONES_H

#include "main.h"
#include "utils/mensajes.h"
#include <utils/mensajes.h>

void guardar_instrucciones_en_memoria(int pid, char* path);
void enviar_instruccion_a_cpu(int pid, int program_counter, int socket);

#endif
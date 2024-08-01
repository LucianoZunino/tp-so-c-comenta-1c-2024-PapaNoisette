#ifndef ESCUCHAR_ENTRADASALIDA_MEMORIA_H
#define ESCUCHAR_ENTRADASALIDA_MEMORIA_H

#include "main.h"

void escuchar_mensajes_entradasalida_memoria(int indice);
void ejecutar_stdin_read(int socket, int pid, int tamanio , int dir_fisica, char* datos);
void ejecutar_stdout_write(int socket, int pid, int tamanio, int dir_fisica, int cod_op);

#endif
#ifndef ESCUCHAR_KERNEL_ENTRADASALIDA_H
#define ESCUCHAR_KERNEL_ENTRADASALIDA_H

#include "main.h"

//void escuchar_mensajes_kernel_entradasalida();
void escuchar_instrucciones_generica();
void escuchar_instrucciones_stdin();
void escuchar_instrucciones_stdout();
void escuchar_instrucciones_dialfs();

void solicitar_almacen_memoria(int direccion, char* mensaje);
void solicitar_lectura_memoria(int direccion, int tamanio);
void notificar_fin(int fd_kernel, int pid);
//void crear_interfaz(char* nombre_interfaz, char* path_config);

#endif
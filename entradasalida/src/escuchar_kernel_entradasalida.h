#ifndef ESCUCHAR_KERNEL_ENTRADASALIDA_H
#define ESCUCHAR_KERNEL_ENTRADASALIDA_H

#include "main.h"

//void escuchar_mensajes_kernel_entradasalida();
void escuchar_instrucciones_generica();
void escuchar_instrucciones_stdin();
void escuchar_instrucciones_stdout();
void escuchar_instrucciones_dialfs();

void solicitar_almacen_memoria(int direccion, char* mensaje, op_code cod_op);
void solicitar_lectura_memoria(int direccion, int tamanio, op_code cod_op);
void notificar_fin(int fd_kernel, int pid);
bool verificar_escritura_archivo(char* path, int reg_tamanio, int reg_puntero_archivo);
void liberar_archivo_bitmap(char* path);
void liberar_bloques_desde_hasta(int inicio, int fin);
void eliminar_segun(char* nombre);
void compactar(char* nombre, t_config* config, int nuevo_tamanio);

//void crear_interfaz(char* nombre_interfaz, char* path_config);

#endif
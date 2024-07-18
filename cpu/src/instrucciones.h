#ifndef CPU_INSTRUCCIONES_H
#define CPU_INSTRUCCIONES_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <utils/utils.h>
#include "main.h"
#include "mmu.h"
#include "iniciar_cpu.h"

#include "ciclo_instrucciones.h"
#include "utils/mensajes.h"


void ejecutar_set(char *registro,int valor);
void ejecutar_mov_in(char* datos,char * direccion);
void ejecutar_mov_out(char* direccion,char * datos);
void ejecutar_sum(char*registro_destino,char *registro_origen);
void ejecutar_sub(char*registro_destino,char *registro_origen);
void ejecutar_jnz(char*registro,char *numero_instruccion);
int  ejecutar_resize(char* tamanio);
void ejecutar_copy_string(char* tamanio);
void ejecutar_wait(char* recurso);
void ejecutar_signal(char* recurso);
void ejecutar_io_gen_sleep(char* interfaz,char * unidades_de_trabajo);
void ejecutar_io_stdin_read(char* interfaz,char * dir_logica, char * tamanio);
void ejecutar_io_stdout_write(char* interfaz,char * dir_logica, char * tamanio);
void ejecutar_io_fs_create(char *interfaz, char *nombre_archivo);
void ejecutar_io_fs_delete(char *interfaz, char *nombre_archivo);
void ejecutar_io_fs_truncate(char *interfaz, char *nombre_archivo, char *reg_tamanio);
void ejecutar_io_fs_write(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo);
void ejecutar_io_fs_read(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo);

#endif

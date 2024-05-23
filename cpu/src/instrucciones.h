#ifndef CPU_INSTRUCCIONES_H
#define CPU_INSTRUCCIONES_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <utils/utils.h>
#include <main.h>
#include "utils.h"
#include "main.h"

#include "ciclo_instrucciones.h"
#include "utils/mensajes.h"


void ejecutar_set(char *registro,char* valor);
void ejecutar_sum(char*registro_destino,char *registro_origen);
void ejecutar_sub(char*registro_destino,char *registro_origen);
void ejecutar_jnz(char*registro,char *numero_instruccion);
void ejecutar_io_gen_sleep(char* interfaz,char * unidades_de_trabajo);
#endif

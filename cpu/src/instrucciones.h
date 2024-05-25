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


void ejecutar_set(char *registro,char* valor,t_proceso_cpu *proceso);
void ejecutar_sum(char*registro_destino,char *registro_origen,t_proceso_cpu *proceso);
void ejecutar_sub(char*registro_destino,char *registro_origen,t_proceso_cpu *proceso);
void ejecutar_jnz(char*registro,char *numero_instruccion,t_proceso_cpu *proceso);
void ejecutar_io_gen_sleep(char* interfaz,char * unidades_de_trabajo);
#endif

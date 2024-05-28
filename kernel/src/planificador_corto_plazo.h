#ifndef PLANIFICADOR_CORTO_PLAZO
#define PLANIFICADOR_CORTO_PLAZO

#include <utils/utils.h>
#include "main.h"
#include "utils_kernel.h"

void planificador_corto_plazo();

void *interrupcion_quantum(void *p);

void esperar_respuesta_de_cpu();

void desalojar_proceso_cpu(op_code motivo, t_pcb* pcb) ;


#endif
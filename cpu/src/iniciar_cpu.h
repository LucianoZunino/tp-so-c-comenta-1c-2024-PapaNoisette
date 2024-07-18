#ifndef INICIAR_CPU_H
#define INICIAR_CPU_H

#include "main.h"
#include <utils/utils.h>

void iniciar_cpu();
void iniciar_logger_cpu();
void iniciar_config_cpu();
void imprimir_config_cpu();
void finalizar_cpu();

//void loggear_ejecucion(t_instruccion *instruccion);//log obligatorio
int get_tamanio_registro(char *registro);
uint32_t get_registro(char *registro);
int consultar_tamanio_pagina_memoria();

#endif

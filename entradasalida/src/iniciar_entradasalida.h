#ifndef INICIAR_ENTRADASALIDA_H
#define INICIAR_ENTRADASALIDA_H

#include "main.h"
#include <utils/utils.h>

void iniciar_entradasalida();
void iniciar_logger_entradasalida();
void iniciar_config_entradasalida(char* config);
void iniciar_estructuras();
void imprimir_config_entradasalida();
void finalizar_entradasalida();
int leer_tipo_interfaz(t_config* config);
#endif
#ifndef INICIAR_ENTRADASALIDA_H
#define INICIAR_ENTRADASALIDA_H

#include "main.h"
#include <utils/utils.h>
#include <sys/mman.h>

void iniciar_entradasalida();
void iniciar_logger_entradasalida();
void iniciar_config_entradasalida(char* config);
void iniciar_estructuras();
void imprimir_config_entradasalida();
void finalizar_entradasalida();
int leer_tipo_interfaz(t_config* config);
void iniciar_lista_metadatas();
char* tomar_nombre_devolver_path(char* nombre);
int redondear_up_con_cero(int divisor, int dividendo);

#endif
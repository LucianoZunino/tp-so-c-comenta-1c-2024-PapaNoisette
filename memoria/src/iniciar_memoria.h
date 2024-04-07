#ifndef INICIAR_MEMORIA_H
#define INICIAR_MEMORIA_H

#include <utils/utils.h>

extern t_log* logger_memoria;
extern t_config* config_memoria;
extern char* puerto_escucha;
extern int tam_memoria;
extern int tam_pagina;
extern char* path_instrucciones;
extern int retardo_respuesta;

void iniciar_memoria();

#endif
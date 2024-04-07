#include "iniciar_memoria.h"

t_log* logger_memoria;
t_config* config_memoria;
char* puerto_escucha;
int tam_memoria;
int tam_pagina;
char* path_instrucciones;
int retardo_respuesta;

void iniciar_logger_memoria(){
    logger_memoria = iniciar_logger("memoria.log", "Memoria");
}

void iniciar_config_memoria(){
    config_memoria = iniciar_config("memoria.config");
    puerto_escucha = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config_memoria, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config_memoria, "TAM_PAGINA");;
    path_instrucciones = config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");;
    retardo_respuesta = config_get_int_value(config_memoria, "RETARDO_RESPUESTA");;
}

void iniciar_memoria(){
    iniciar_logger_memoria();
    iniciar_config_memoria();
    //imprimir_config()?
}

void finalizar_memoria(){ // capaz debería estar en otro archivo?
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
}
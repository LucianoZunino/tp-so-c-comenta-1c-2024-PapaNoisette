#include "iniciar_memoria.h"

t_config* config_memoria;


void iniciar_logger_memoria(){
    logger_memoria = iniciar_logger("memoria.log", "Memoria");
}

void iniciar_config_memoria(){
    config_memoria = iniciar_config("memoria.config");
    puerto_escucha = config_get_int_value(config_memoria, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config_memoria, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config_memoria, "TAM_PAGINA");;
    path_instrucciones = config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");;
    retardo_respuesta = config_get_int_value(config_memoria, "RETARDO_RESPUESTA");;

}
void print_config(){
    printf("puerto_escucha:%d\n",puerto_escucha);
    printf("tam_memoria:%d\n",tam_memoria);
    printf("tam_pagina:%d\n",tam_pagina);
    printf("path_instrucciones:%s\n",path_instrucciones);
    printf("retardo_respuesta:%d\n",retardo_respuesta);
}

void iniciar_memoria(){
    iniciar_logger_memoria();
    iniciar_config_memoria();
    print_config();

}

void finalizar_memoria(){ // capaz debería estar en otro archivo?
    log_destroy(logger_memoria);
    config_destroy(config_memoria);

}
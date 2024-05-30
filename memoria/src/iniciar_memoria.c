#include "iniciar_memoria.h"

t_config* config_memoria;
t_list* lista_de_miniPcb;
pthread_mutex_t* mutex_miniPcb;

void iniciar_memoria(){
    iniciar_logger_memoria();
    iniciar_config_memoria();
    //imprimir_config_memoria();
}

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

void imprimir_config_memoria(){
    printf("PUERTO_ESCUCHA:%s\n",puerto_escucha);
    printf("TAM_MEMORIA:%d\n",tam_memoria);
    printf("TAM_PAGINA:%d\n",tam_pagina);
    printf("PATH_INSTRUCCIONES:%s\n",path_instrucciones);
    printf("RETARDO_RESPUESTA:%d\n",retardo_respuesta);
}

void finalizar_memoria(){
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
}

void inicializar_estructuras(){
    t_list *lista_de_miniPcb = list_create();
}

void inicializar_semaforos(){
    if (pthread_mutex_init(&mutex_miniPcb, NULL) != 0) {
        log_error(logger_memoria, "No se pudo inicializar el semaforo para la lista miniPcb");
        exit(-1);
    } 
}
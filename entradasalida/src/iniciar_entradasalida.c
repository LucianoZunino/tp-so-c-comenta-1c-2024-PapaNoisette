#include "iniciar_entradasalida.h"

t_config* config_entradasalida;

void iniciar_entradasalida(){
    iniciar_logger_entradasalida();
    iniciar_config_entradasalida();
    iniciar_estructuras();
    //imprimir_config_entradasalida();
}

void iniciar_logger_entradasalida(){
    logger_entradasalida = iniciar_logger("entradasalida.log", "entradasalida");
}

void iniciar_config_entradasalida(){
    config_entradasalida = iniciar_config("entradasalida.config");

    tipo_de_interfaz = config_get_string_value(config_entradasalida, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config_entradasalida, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config_entradasalida, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value(config_entradasalida, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config_entradasalida, "BLOCK_SIZE");
    block_count = config_get_int_value(config_entradasalida, "BLOCK_COUNT");
}

void imprimir_config_entradasalida(){
    printf("TIPO_INTERFAZ:%s\n",tipo_de_interfaz);
    printf("TIEMPO_UNIDAD_TRABAJO:%d\n",tiempo_unidad_trabajo);
    printf("IP_KERNEL:%s\n",ip_kernel);
    printf("PUERTO_KERNEL:%s\n",puerto_kernel);
    printf("IP_MEMORIA:%s\n",ip_memoria);
    printf("PUERTO_MEMORIA:%s\n",puerto_memoria);
    printf("PATH_BASE_DIALFS:%s\n",path_base_dialfs);
    printf("BLOCK_SIZE:%d\n",block_size);
    printf("BLOCK_COUNT:%d\n",block_count);
}

void iniciar_estructuras(){
    interfaces = dictionary_create();
}

void finalizar_entradasalida(){
    log_destroy(logger_entradasalida);
    config_destroy(config_entradasalida);
}
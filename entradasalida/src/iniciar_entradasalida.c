#include "iniciar_entradasalida.h"

t_config* config_entradasalida;
sem_t sem_stdout;

void iniciar_entradasalida(){
    iniciar_logger_entradasalida();
    iniciar_config_entradasalida("entradasalida.config");
    iniciar_estructuras();
    //imprimir_config_entradasalida();
}

void iniciar_logger_entradasalida(){
    logger_entradasalida = iniciar_logger("entradasalida.log", "entradasalida");
}

void iniciar_config_entradasalida(char* config){
    printf("INICIAR CONFIG ENTRADASALIDA /n");
    config_entradasalida = iniciar_config(config);

    ip_kernel = config_get_string_value(config_entradasalida, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config_entradasalida, "PUERTO_KERNEL");
    tipo_de_interfaz = leer_tipo_interfaz(config_entradasalida);
   // log_info(logger_auxiliar, "Tipo interfaz: %d", TIPO_INTERFAZ);
    switch (tipo_de_interfaz) {
    case GENERICA:
        tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");            
        break;
    case STDIN:
        ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
        puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
        break;
    case STDOUT:
        tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
        ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
        puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
        break;
    case DIAL_FS:
        tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
        ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
        puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
        path_base_dialfs = config_get_string_value(config_entradasalida, "PATH_BASE_DIALFS");
        block_size = config_get_int_value(config_entradasalida, "BLOCK_SIZE");
        block_count = config_get_int_value(config_entradasalida, "BLOCK_COUNT");
        retraso_compactacion = config_get_int_value(config_entradasalida, "RETRASO_COMPACTACION");
        break;   
    }
}

void imprimir_config_entradasalida(){
    printf("TIPO_INTERFAZ:%i\n",tipo_de_interfaz);
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
    sem_init(&sem_stdout, 1, 0);
}

void finalizar_entradasalida(){
    log_destroy(logger_entradasalida);
    config_destroy(config_entradasalida);
}

int leer_tipo_interfaz(t_config* config_entradasalida){
        char* tipo = config_get_string_value(config_entradasalida, "TIPO_INTERFAZ");
    if ( string_equals_ignore_case(tipo, "GENERICA") ) {
        return GENERICA;
    } else if ( string_equals_ignore_case(tipo, "STDIN") ) {
        return STDIN;
    } else if ( string_equals_ignore_case(tipo, "STDOUT") ) {
        return STDOUT;
    } else if ( string_equals_ignore_case(tipo, "DIALFS") ) {
        return DIAL_FS;
    } else {
        log_error(logger_entradasalida, "Tipo de interfaz no reconocida: %s", tipo);
        return (-1);
    }
}
#include "iniciar_entradasalida.h"


t_config* config_entradasalida;


void iniciar_logger_entradasalida(){
    logger_entradasalida = iniciar_logger("entradasalida.log", "entradasalida");
}

void iniciar_config_entradasalida(){
    config_entradasalida = iniciar_config("entradasalida.config");

    
     tipo_interfaz = config_get_string_value(config_entradasalida, "TIPO_INTERFAZ");
     tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
     ip_kernel = config_get_string_value(config_entradasalida, "IP_KERNEL");
     puerto_kernel = config_get_int_value(config_entradasalida, "PUERTO_KERNEL");
     ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
     puerto_memoria = config_get_int_value(config_entradasalida, "PUERTO_MEMORIA");
     path_base_dialfs = config_get_string_value(config_entradasalida, "PATH_BASE_DIALFS");
     block_size = config_get_int_value(config_entradasalida, "BLOCK_SIZE");
     block_count = config_get_int_value(config_entradasalida, "BLOCK_COUNT");



}

void iniciar_entradasalida(){
    iniciar_logger_entradasalida();
    iniciar_config_entradasalida();
    print_config();
}

void print_config(){

    printf("tipo_interfaz:%s\n",tipo_interfaz);
    printf("tiempo_unidad_trabajo:%d\n",tiempo_unidad_trabajo);
    printf("ip_kernel:%s\n",ip_kernel);
    printf("puerto_kernel:%d\n",puerto_kernel);
    printf("ip_memoria:%s\n",ip_memoria);
    printf("puerto_memoria:%d\n",puerto_memoria);
    printf("path_base_dialfs:%s\n",path_base_dialfs);
    printf("block_size:%d\n",block_size);
    printf("block_count:%d\n",block_count);


}

void finalizar_entradasalida(){ // capaz debería estar en otro archivo?//no asi esta ok(nacho)
    log_destroy(logger_entradasalida);
    config_destroy(config_entradasalida);
}
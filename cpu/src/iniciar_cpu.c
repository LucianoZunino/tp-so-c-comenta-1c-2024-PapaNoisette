#include "iniciar_cpu.h"


t_config* config_cpu;


void iniciar_logger_cpu(){
    logger_cpu = iniciar_logger("cpu.log", "cpu");
}

void iniciar_config_cpu(){
    config_cpu = iniciar_config("cpu.config");

     
    ip_memoria = config_get_string_value(config_cpu, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_cpu, "PUERTO_MEMORIA");
    puerto_escucha_dispatch = config_get_string_value(config_cpu, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_string_value(config_cpu, "PUERTO_ESCUCHA_INTERRUPT");
    cantidad_entradas_tlb = config_get_int_value(config_cpu, "CANTIDAD_ENTRADAS_TLB");
    algoritmo_tlb = config_get_string_value(config_cpu, "ALGORITMO_TLB");
}



void imprimir_config_cpu(){
    printf("ip_memoria:%s\n",ip_memoria);
    printf("puerto_memoria:%s\n",puerto_memoria);
    printf("puerto_escucha_dispatch:%s\n",puerto_escucha_dispatch);
    printf("puerto_escucha_interrupt:%s\n",puerto_escucha_interrupt);
    printf("cantidad_entradas_tlb:%d\n",cantidad_entradas_tlb);
    printf("algoritmo_tlb:%s\n",algoritmo_tlb);
}

void iniciar_cpu(){
    iniciar_logger_cpu();
    iniciar_config_cpu();
    imprimir_config_cpu();
}
void finalizar_cpu(){ 
    config_destroy(config_cpu);
    log_destroy(logger_cpu);

}
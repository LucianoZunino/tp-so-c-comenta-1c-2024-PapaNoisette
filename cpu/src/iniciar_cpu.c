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



void print_config(){
    printf("IP_MEMORIA:%s\n",ip_memoria);
    printf("PUERTO_MEMORIA:%s\n",puerto_memoria);
    printf("PUERTO_ESCUCHA_DISPATCH:%s\n",puerto_escucha_dispatch);
    printf("PUERTO_ESCUCHA_INTERRUPT:%s\n",puerto_escucha_interrupt);
    printf("CANTIDAD_ENTRADAS_TLB:%d\n",cantidad_entradas_tlb);
    printf("ALGORITMO_TLB:%s\n",algoritmo_tlb);
}

void iniciar_cpu(){
    iniciar_logger_cpu();
    iniciar_config_cpu();
    print_config();
}
void finalizar_cpu(){ 
    log_destroy(logger_cpu);
    config_destroy(config_cpu);
}
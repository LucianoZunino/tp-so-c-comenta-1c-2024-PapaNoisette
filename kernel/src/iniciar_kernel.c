#include "iniciar_kernel.h"

t_config* config_kernel;

void iniciar_kernel(){
    iniciar_logger_kernel();
    iniciar_config_kernel();
    iniciar_semaforos();
    //imprimir_config_kernel();
}

void iniciar_logger_kernel(){
    logger_kernel = iniciar_logger("kernel.log", "kernel");
}

void iniciar_config_kernel(){
    config_kernel = iniciar_config("kernel.config");

    puerto_escucha = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config_kernel, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config_kernel, "PUERTO_CPU_INTERRUPT");
    algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
    quantum = config_get_int_value(config_kernel, "QUANTUM");
    recursos = config_get_array_value(config_kernel, "RECURSOS");
    instancias_recursos = config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");
}

void imprimir_config_kernel(){
    printf("PUERTO_ESCUCHA: %s\n",puerto_escucha);
    printf("IP_MEMORIA: %s\n",ip_memoria);
    printf("PUERTO_MEMORIA: %s\n",puerto_memoria);
    printf("IP_CPU: %s\n",ip_cpu);
    printf("PUERTO_CPU_DISPATCH: %s\n",puerto_cpu_dispatch);
    printf("PUERTO_CPU_INTERRUPT: %s\n",puerto_cpu_interrupt);
    printf("ALGORITMO_PLANIFICACION: %s\n",algoritmo_planificacion);
    printf("QUANTUM: %d\n",quantum);
    for(int i=0; i<largo_array(recursos); i++){
        printf("RECURSOS: %s\n",recursos[i]);
    }
    for(int i=0; i<largo_array(instancias_recursos); i++){
        printf("INSTANCIAS_RECURSOS: %s\n",instancias_recursos[i]);
    }
    printf("GRADO_MULTIPROGRAMACION: %d\n",grado_multiprogramacion);
}

void iniciar_semaforos(){
    
}

void finalizar_kernel(){
    log_destroy(logger_kernel);
    config_destroy(config_kernel);
}
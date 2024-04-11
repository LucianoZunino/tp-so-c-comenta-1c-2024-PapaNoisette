#include "iniciar_kernel.h"


t_config* config_kernel;


void iniciar_logger_kernel(){
    logger_kernel = iniciar_logger("kernel.log", "kernel");
}

void iniciar_config_kernel(){
    config_kernel = iniciar_config("kernel.config");

     puerto_escucha = config_get_int_value(config_kernel, "PUERTO_ESCUCHA");
     ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
     puerto_memoria = config_get_int_value(config_kernel, "PUERTO_MEMORIA");
     ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
     puerto_cpu_dispatch = config_get_int_value(config_kernel, "PUERTO_CPU_DISPATCH");
     puerto_cpu_interrupt = config_get_int_value(config_kernel, "PUERTO_CPU_INTERRUPT");
     algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
     quantum = config_get_int_value(config_kernel, "QUANTUM");
     recursos = config_get_array_value(config_kernel, "RECURSOS");
     instancias_recursos = config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS");
     grado_multiprogramacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");

}

void print_config(){
    printf("puerto_escucha:%d\n",puerto_escucha);
    printf("ip_memoria:%s\n",ip_memoria);
    printf("puerto_memoria:%d\n",puerto_memoria);
    printf("ip_cpu:%s\n",ip_cpu);
    printf("puerto_cpu_dispatch:%d\n",puerto_cpu_dispatch);
    printf("puerto_cpu_interrupt:%d\n",puerto_cpu_interrupt);
    printf("algoritmo_planificacion:%s\n",algoritmo_planificacion);
    printf("quantum:%d\n",quantum);
    for (int i=0;i<largo_array(recursos);i++){
    printf("recursos:%s\n",recursos[i]);
    }
    for (int i=0;i<largo_array(instancias_recursos);i++){
    printf("instancias_recursos:%s\n",instancias_recursos[i]);
    }
    printf("grado_multiprogramacion:%d\n",grado_multiprogramacion);

}

void iniciar_kernel(){
    iniciar_logger_kernel();
    iniciar_config_kernel();
    print_config();
}


void finalizar_kernel(){ // capaz debería estar en otro archivo?//no asi esta ok(nacho)
    log_destroy(logger_kernel);
    config_destroy(config_kernel);
}
#include "iniciar_interfaz.h"

void* iniciar_interfaz_generica(t_interfaz* interfaz){
    sem_wait(&interfaz -> semaforo);


    usleep(config_get_int_value(interfaz -> config, "TIEMPO_UNIDAD_TRABAJO"));

    // TODO Notificar a kernel fin de sleep
}
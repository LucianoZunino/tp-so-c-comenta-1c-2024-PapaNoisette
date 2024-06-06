#include "planificador_largo_plazo.h"
pthread_mutex_t mutex_multiprogramacion;
pthread_mutex_t mutex_READY;
//sem_t sem_READY;

void planificador_largo_plazo() {
    while (true){
        
        sem_wait(&sem_NEW);


        sem_wait(&sem_MULTIPROGRAMACION); // cuando un estado sale de BLOCKED, READY ó RUNNING, hace un signal de este semáforo
            
        pthread_mutex_lock(&mutex_NEW);
        t_pcb *new_pcb = list_remove(NEW, 0);
        pthread_mutex_unlock(&mutex_NEW);
        new_pcb->estado = E_READY;

        pthread_mutex_lock(&mutex_multiprogramacion); 
        grado_actual_multiprogramacion++;              // haria falta? 
        pthread_mutex_unlock(&mutex_multiprogramacion);

        pthread_mutex_lock(&mutex_READY);
        queue_push(READY, new_pcb);
        pthread_mutex_unlock(&mutex_READY);
        sem_post(&sem_READY);
        

       
    }
}

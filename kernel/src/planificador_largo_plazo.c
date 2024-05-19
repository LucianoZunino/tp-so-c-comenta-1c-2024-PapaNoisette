#include "planificador_largo_plazo.h"
pthread_mutex_t mutex_multiprogramacion;
pthread_mutex_t mutex_READY;
//sem_t sem_READY;


void planificador_largo_plazo() {
    while (true){
        
        sem_wait(&sem_NEW);
        pthread_mutex_lock(&mutex_NEW);

        // PASO de NEW a READY - Largo Plazo
        //sem_wait(&semaforo_mp);
        //pthread_mutex_lock(&mutex_mp);
        if (list_size(NEW) > 0 && grado_actual_multiprogramacion < grado_multiprogramacion) // Con los semaforos ya no haria falta
        {
            
            //pthread_mutex_unlock(&mutex_mp);
            t_pcb *new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);
            new_pcb->estado = E_READY;

            pthread_mutex_lock(&mutex_multiprogramacion);
            grado_actual_multiprogramacion++;
            pthread_mutex_unlock(&mutex_multiprogramacion);

            pthread_mutex_lock(&mutex_READY);
            queue_push(READY, new_pcb);
            pthread_mutex_unlock(&mutex_READY);
            sem_post(&sem_READY);
            //encolar_proceso(new_pcb, READY, &mutex_READY, "NEW", "READY");
        }
        else
        {
            pthread_mutex_unlock(&mutex_NEW);
            //pthread_mutex_unlock(&mutex_mp);
        }
    }
}
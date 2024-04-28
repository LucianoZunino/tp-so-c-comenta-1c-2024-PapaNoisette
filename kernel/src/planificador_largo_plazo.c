#include "planificador_largo_plazo.h"

void planificador_largo_plazo() {
    while (true){
        //sem_wait(&semaforo_NEW);
        pthread_mutex_lock(&mutex_NEW);

        // PASO de NEW a READY - Largo Plazo
        //pthread_mutex_lock(&mutex_mp);
        //sem_wait(&semaforo_mp);
        if (list_size(NEW) > 0 && grado_actual_multiprogramacion < grado_multiprogramacion) // Con los semaforos ya no haria falta
        {
            grado_actual_multiprogramacion++;
            //pthread_mutex_unlock(&mutex_mp);
            t_pcb *new_pcb = list_remove(NEW, 0);
            pthread_mutex_unlock(&mutex_NEW);
            //new_pcb->tabla_segmentos = solicitar_tabla_segmentos(new_pcb->pid);
            //encolar_proceso(new_pcb, READY, &mutex_READY, "NEW", "READY");
        }
        else
        {
            pthread_mutex_unlock(&mutex_NEW);
            //pthread_mutex_unlock(&mutex_mp);
        }
    }
}
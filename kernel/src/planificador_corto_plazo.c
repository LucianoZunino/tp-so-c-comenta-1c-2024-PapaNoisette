#include "planificador_corto_plazo.h"
#include <pthread.h>


t_pcb *RUNNING;
pthread_mutex_t mutex_RUNNING;
//pthread_mutex_t mutex_READY;
sem_t sem_EXEC;
//sem_t sem_READY;

void planificador_corto_plazo(){
    while(true){
        sem_wait(&sem_READY);
        sem_wait(&sem_EXEC); // sem_signal deberia estar en EXEC (cpu) y que avise cuando desaloja el proceso en ejecucion

        if( algoritmo_planificacion == "FIFO" || algoritmo_planificacion == "RR"){
            
            t_pcb *pcb = queue_pop(READY);
            pcb->estado = E_EXEC;
            enviar_proceso_por_paquete(pcb, NULL ,fd_cpu_dispatch, ENVIAR_PROCESO_A_EXEC);
            // recibir OK de CPU
            
            pthread_mutex_lock(&mutex_RUNNING);
            RUNNING = pcb;
            pthread_mutex_unlock(&mutex_RUNNING);

            if(algoritmo_planificacion == "RR"){
                iniciar_quantum(pcb);
            }
        } else {
        // ROUND ROBIN VIRTUAL
        //}

        /* PSEUDOCODIGO DE LO QUE FALTA MAS O MENOS
        
        mandar_a_cpu();
        void *buffer = recibir_nuevo_contexto(socket_cpu, &cop);
        deserializar_contexto_pcb(buffer, r_pcb);
        
        */


        }
    }
}

void iniciar_quantum(t_pcb *pcb){
    
    pthread_t hilo_quantum;
    pthread_create(&hilo_quantum, NULL, interrupcion_quantum, pcb);
    pthread_detach(hilo_quantum);
}

void *interrupcion_quantum(void *p){
    t_pcb *pcb = (t_pcb *)p;
    usleep(quantum * 1000);
    interrumpir_cpu(pcb, FIN_DE_QUANTUM);
    pcb_destruir(pcb);
    
}

#include "planificador_corto_plazo.h"
sem_t sem_EXEC;

void planificador_corto_plazo(){
    while(true){
        
        sem_wait(&sem_EXEC); // sem_signal deberia estar en EXEC (cpu) y que avise cuando desaloja el proceso en ejecucion

        if( algoritmo_planificacion == "FIFO" || algoritmo_planificacion == "RR"){
            
            t_pcb *pcb = queue_pop(READY);
            pcb->estado = E_EXEC;
            enviar_proceso_por_paquete(pcb, NULL ,fd_cpu_dispatch, ENVIAR_PROCESO_A_EXEC);
            
            pthread_mutex_lock(&mutex_RUNNING);
            RUNNING = pcb;
            pthread_mutex_unlock(&mutex_RUNNING);

        }//else{
            // ROUND ROBIN VIRTUAL
        //}


    }

}
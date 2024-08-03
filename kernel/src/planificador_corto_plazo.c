#include "planificador_corto_plazo.h"
#include <pthread.h>
#include <commons/temporal.h>


t_pcb *RUNNING = NULL;
pthread_mutex_t mutex_RUNNING;
sem_t sem_EXEC;


void planificador_corto_plazo(){
    while(true){

        sem_wait(&sem_READY);

        sem_wait(&sem_EXEC);

        if(flag_planificacion_detenido){
        
            sem_wait(&sem_planificador_CP_detenido);
        }

        if(string_equals_ignore_case(algoritmo_planificacion,"RR") || string_equals_ignore_case(algoritmo_planificacion, "FIFO")){

            pthread_mutex_lock(&mutex_READY);
            t_pcb *pcb = list_remove(READY, 0);
            pthread_mutex_unlock(&mutex_READY);
            cambio_de_estado(pcb, E_EXEC);
            //pcb->estado = E_EXEC;
            pthread_mutex_lock(&mutex_RUNNING);
            RUNNING = pcb;
            pthread_mutex_unlock(&mutex_RUNNING);
            enviar_proceso_cpu(pcb, fd_cpu_dispatch, KERNEL_ENVIA_PROCESO);

            if(string_equals_ignore_case(algoritmo_planificacion,"RR")){
                esperar_a_cpu_round_robin(pcb);
            }
                
        }
        else if(string_equals_ignore_case(algoritmo_planificacion,"VRR")){
            
            t_pcb* pcb;

            if(list_is_empty(PRIORIDAD)){
                
                pthread_mutex_lock(&mutex_READY);
                pcb = list_remove(READY, 0);
                pthread_mutex_unlock(&mutex_READY);
                cambio_de_estado(pcb, E_EXEC);
            
                pthread_mutex_lock(&mutex_RUNNING);
                RUNNING = pcb;
                pthread_mutex_unlock(&mutex_RUNNING);
            }
            else{
                pthread_mutex_lock(&mutex_PRIORIDAD);
                pcb = list_remove(PRIORIDAD, 0);
                pthread_mutex_unlock(&mutex_PRIORIDAD);
                cambio_de_estado(pcb, E_EXEC);
        
               // pcb->estado = E_EXEC;
            
                pthread_mutex_lock(&mutex_RUNNING);
                RUNNING = pcb;
                pthread_mutex_unlock(&mutex_RUNNING);
            }
            
            pthread_t hilo_paquete_cpu;
            enviar_proceso_cpu(pcb, fd_cpu_dispatch, KERNEL_ENVIA_PROCESO);// Pasar a un hilo
           
            int64_t ms_transcurridos = esperar_a_cpu_virtual_round_robin(RUNNING);
            printf("\n ms_transcurridos: %i\n", ms_transcurridos);

            if(RUNNING && ms_transcurridos < RUNNING->quantum){
 
                pthread_mutex_lock(&mutex_RUNNING);
                RUNNING->quantum = RUNNING->quantum - ms_transcurridos;
                pthread_mutex_unlock(&mutex_RUNNING);
                
            }

            sem_post(&sem_quantum);
        }
        else{
            log_error(logger_kernel, "El Algoritmo de planificacion no es correcto");
        }
        // esperar_respuesta_de_CPU con: contexto, motivo_desalojo
    }
}

void hilo_quantum_funcion(t_pcb* pcb){
    sem_wait(&sem_paqueteCPU);
    usleep((pcb->quantum) * 1000);
    send_interrupt();
}

void esperar_a_cpu_round_robin(t_pcb* pcb){
    
    pthread_t hilo_quantum;
    pthread_create(&hilo_quantum, NULL, (void*) hilo_quantum_funcion, pcb);
    pthread_detach(hilo_quantum);
    
    
    sem_wait(&sem_desalojo); //esperar_a_que_cpu_desaloje(pcb); por fin de Quantum o por otro motivo
   

    pthread_cancel(hilo_quantum);
  
}

int64_t esperar_a_cpu_virtual_round_robin(t_pcb* pcb){
    t_temporal* timer = temporal_create();
    esperar_a_cpu_round_robin(pcb);
    temporal_stop(timer);
    int64_t ms_transcurridos = temporal_gettime(timer);
    temporal_destroy(timer);

    return ms_transcurridos;
}

void send_interrupt(){
   
    t_buffer* buffer = crear_buffer();
    t_paquete* paquete = crear_paquete(FIN_DE_QUANTUM,buffer);
    cargar_int_al_buffer(paquete->buffer, RUNNING->pid);
    cargar_int_al_buffer(paquete->buffer, FIN_DE_QUANTUM);
    enviar_paquete(paquete, fd_cpu_interrupt);
    eliminar_paquete(paquete);
}

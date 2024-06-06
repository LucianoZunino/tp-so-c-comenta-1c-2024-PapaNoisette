#include "planificador_corto_plazo.h"
#include <pthread.h>
#include <commons/temporal.h>


t_pcb *RUNNING = NULL;
pthread_mutex_t mutex_RUNNING;
//pthread_mutex_t mutex_READY;
sem_t sem_EXEC;
//sem_t sem_READY;

void planificador_corto_plazo(){
    while(true){
        sem_wait(&sem_READY);
        sem_wait(&sem_EXEC); // sem_signal deberia estar en EXEC (cpu) y que avise cuando desaloja el proceso en ejecucion


        if(string_equals_ignore_case(algoritmo_planificacion,"RR") || string_equals_ignore_case(algoritmo_planificacion, "FIFO")){
            t_pcb *pcb = queue_pop(READY);
            pcb->estado = E_EXEC;

            // Lo comento para testear nomas, asi no rompe, pq falta implementarlo en cpu
            enviar_proceso_por_paquete(pcb, NULL ,fd_cpu_dispatch, KERNEL_ENVIA_PROCESO); 
            // recibir OK de CPU
            
            pthread_mutex_lock(&mutex_RUNNING);
            RUNNING = pcb;
            pthread_mutex_unlock(&mutex_RUNNING);

            if(algoritmo_planificacion == "RR"){
                //iniciar_quantum(pcb);
            }
        } else if(string_equals_ignore_case(algoritmo_planificacion,"RR")){
            // VIRTUAL ROUND ROBIN

            // PSEUDOCODIGO DE LO QUE FALTA MAS O MENOS
            
            
            
            //
        }
        else {
            log_error(logger_kernel, "El Algoritmo de planificacion no es correcto");
        }

        // 
        // esperar_respuesta_de_CPU con: contexto, motivo_desalojo

       
    }
}

/*void esperar_respuesta_de_cpu()
{
    op_code motivo = recibir_operacion (fd_cpu_dispatch);
    
    t_buffer *buffer = crear_buffer(fd_cpu_dispatch);
        
    buffer = recibir_buffer_completo(fd_cpu_dispatch);
        
    t_pcb* pcb = deserializar_pcb(buffer);

    destruir_buffer(buffer);

    desalojar_proceso_cpu (motivo, pcb);
}


void desalojar_proceso_cpu(op_code motivo, t_pcb* pcb) 
{
    switch (motivo)
    {
    case RECURSOS:
        pasar_proceso_a_ready(pcb);
        break;
    
    case EXIT:
        break;
    

    default:
        break;
    }
}
*/
void pasar_proceso_a_ready(t_pcb* pcb){
    
    pthread_mutex_lock(&mutex_READY);
    queue_push(READY, pcb);
    pthread_mutex_unlock(&mutex_READY);
    
    sem_post(&sem_READY);
    sem_post(&sem_EXEC);
}

/*
void interrumpir_cpu(motivo_interrupcion motivo){
    t_buffer* buffer_interrupt = crear_buffer();
    t_paquete* paquete = crear_paquete(INTERRUPCION, buffer_interrupt);
    cargar_int_al_buffer(paquete->buffer, motivo);
    enviar_paquete(paquete, fd_cpu_interrupt);
    eliminar_paquete(paquete);
    
}
*/
/*

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
timer = temporal_create();
do
{
    if (DONE_BEFORE_QUANTUM)
    {
        temporal_stop(timer);
        time_elapsed = temporal_gettime(timer);
        temporal_destroy(temporal);
        break;
    }
} while (temporal_gettime(timer) < pcb->quantum);
if(!DONE_BEFORE_QUANTUM) send_interrupt();
    temporal_destroy(timer);
    


void hilo_quantum(pcb){
  sleep(pcb->quantum);
  send_interrupt();
}

void esperar_a_cpu_round_robin(pcb){
    pthread_create(hilo_quantum);
    pthread_detach(hilo_quantum);
    esperar_a_que_cpu_desaloje(pcb); // por fin de Quantum o por otro motivo
    pthread_cancel(hilo_quantum);
}

void esperar_a_cpu_vritual_round_robin(pcb){
    t_temporal* timer = temporal_create();
    esperar_a_cpu_round_robin(pcb);
    temporal_stop(timer);
    int64_t ms_transcurridos = temporal_gettime(timer);
    temporal_destroy(timer);
}
*/
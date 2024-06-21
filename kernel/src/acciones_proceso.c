#include "acciones_proceso.h"
uint32_t next_pid = 0;
pthread_mutex_t mutex_next_pid;
pthread_mutex_t mutex_NEW;
pthread_mutex_t mutex_EXIT;
pthread_mutex_t socket_memoria_mutex;
//sem_t sem_EXIT;


//sem_t sem_NEW;
void imprimir_new(){
    // INICIAR_PROCESO path.txt
    printf("Cantidad de procesos en NEW: %i\n", list_size(NEW));
    for (unsigned i = 0; i < list_size(NEW); i++){
       t_pcb* pcb = (t_pcb*) list_get(NEW, i);
       printf("PID: %i\n", pcb->pid);
    }
    printf("Cantidad de procesos en READY: %i\n", list_size(READY));
    if (RUNNING != NULL)
        printf("Proceso en RUNNING: %i\n", RUNNING->pid);
    else 
        printf("Ningun proceso en RUNNING\n");
}

void crear_proceso(char* path){
    
    t_pcb *nuevo_pcb = crear_pcb();

    nuevo_pcb->estado = E_NEW;
    printf("Pcb encolado en new, PID: %i\n", nuevo_pcb->pid);
    pthread_mutex_lock(&mutex_NEW);
    list_add(NEW, nuevo_pcb);
    pthread_mutex_unlock(&mutex_NEW);


    sem_post(&sem_NEW); // no deberia ser despues de recibir el ok de memoria? QUIEN HACE UN SEM_WAIT?

    enviar_proceso_por_paquete(nuevo_pcb, path, fd_memoria, MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS);
    // printf("Crear proceso\n");


    // op_code codigo_operacion = recibir_operacion(fd_memoria);
    // if(codigo_operacion != KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS)
    // {
    //     log_error(logger_kernel, "PID %i - No se pudo inicializar estructuras", nuevo_pcb->pid);
    //     return;
    // }
    // // Hace falta? 
    // recibir_ok (fd_memoria);
    
    sem_wait(&sem_estructuras_inicializadas);

    //TEST */

    // TODO: Revisar tema del PID para tener el log bien hecho
    log_info(logger_kernel, "Se crea el proceso %i en NEW", nuevo_pcb->pid);


    imprimir_new();
}



t_pcb *crear_pcb(){

    t_pcb *pcb = malloc(sizeof(t_pcb));

    pthread_mutex_lock(&mutex_next_pid);
    pcb->pid = next_pid;
    next_pid++;
    pthread_mutex_unlock(&mutex_next_pid);

    pcb->program_counter = 0;

    t_registros_cpu *registros_cpu = malloc(sizeof(t_registros_cpu));
    

    pcb->registros_cpu = registros_cpu;

    int quantum = malloc(sizeof(int));
    
    pcb->quantum = quantum;

    pcb->estado = malloc(sizeof(pcb->estado));



    // ESTA MEMORIA SE ELIMINA EN ELIMINAR_PROCESO

    return pcb;
}



void eliminar_proceso() {
    sem_wait(&sem_EXIT);
    
    pthread_mutex_lock(&mutex_EXIT);
    t_pcb* pcb = list_remove(EXIT, 0);
    pthread_mutex_unlock(&mutex_EXIT);
    int pid = pcb->pid;
    
    solicitar_liberar_en_memoria(pid);
    
    pcb_destruir(pcb); // cada pcb tendría que tener referencias a los recursos asignados al proceso // liberar_recursos();

    pthread_mutex_lock(&mutex_multiprogramacion);
    grado_actual_multiprogramacion--;
    pthread_mutex_unlock(&mutex_multiprogramacion);

    // TODO: Liberar recursos
    /*
    typedef struct{
    char* nombre;
    int instancias; (actuales)
    t_queue* cola_de_espera;
    t_list* pcb_asignados;
    pthread_mutex_t mutex;
}t_recurso;
    */
    liberar_recursos_de(pcb);
    
    sem_post(&sem_MULTIPROGRAMACION);
}

void solicitar_liberar_en_memoria(int pid) {
    t_buffer* buffer = crear_buffer();
    t_paquete* paquete = crear_paquete(LIBERAR_PROCESO_EN_MEMORIA, buffer);
    cargar_int_al_buffer(paquete->buffer, pid);
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);
}


void liberar_recursos_de(t_pcb* pcb) {
    
}
#include "acciones_proceso.h"
uint32_t next_pid = 0;
pthread_mutex_t mutex_next_pid;
pthread_mutex_t mutex_NEW;
pthread_mutex_t socket_memoria_mutex;


//sem_t sem_NEW;
void imprimir_new(){
    // INICIAR_PROCESO path.txt
    printf("Cantidad de procesos en NEW: %i\n", list_size(NEW));
    for (unsigned i = 0; i < list_size(NEW); i++){
       t_pcb* pcb = (t_pcb*) list_get(NEW, i);
       printf("PID: %i\n", pcb->pid);
    }
    printf("Cantidad de procesos en READY: %i\n", queue_size(READY));
}

void crear_proceso(char* path){

    t_pcb *nuevo_pcb = crear_pcb();

    nuevo_pcb->estado = E_NEW;
    printf("Pcb encolado en new, PID: %i\n", nuevo_pcb->pid);
    pthread_mutex_lock(&mutex_NEW);
    list_add(NEW, nuevo_pcb);
    pthread_mutex_unlock(&mutex_NEW);

    //t_pcb* pcb_index_0 = (t_pcb*) list_get(NEW, 0);
    //printf("PID: %i\n", pcb_index_0->pid);

    //t_pcb* pcb_index_00 = (t_pcb*) list_get(NEW, 0);
    //printf("PID: %i\n", pcb_index_00->pid);

    imprimir_new();

    sem_post(&sem_NEW); // no deberia ser despues de recibir el ok de memoria?

    //pthread_mutex_lock(&socket_memoria_mutex); SUPONEMOS QUE SE TRABAJA EN PARALELO AL SOCKET PARA VER SI NECESITA MUTEX
    enviar_proceso_por_paquete(nuevo_pcb, path, fd_memoria, MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS);
    // printf("Crear proceso\n");

    /* TEST
    op_code codigo_operacion = recibir_codigo_operacion(fd_memoria);
    if(codigo_operacion != KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS)
    {
        log_error(logger_kernel, "PID %i - No se pudo inicializar estructuras", nuevo_pcb->pid);
        return;
    }
    // Hace falta? 
    // recibir_kernel_respuesta_inicializar_estructuras(fd_memoria);

    //pthread_mutex_unlock(&socket_memoria_mutex);

    TEST */

    // TODO: Revisar tema del PID para tener el log bien hecho
    log_info(logger_kernel, "Se crea el proceso %i en NEW", nuevo_pcb->pid);


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

    // ESTA MEMORIA SE ELIMINA EN ELIMINAR_PROCESO

    return pcb;
}



    /*FILE* archivo = fopen(path, "r");

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", path);
        return;
    }
    
    char* instrucciones_del_path = list_create();
    
    char* linea[100]; 
    
    while(fgets(linea, sizeof(linea), archivo) != NULL){
        
        
        // Eliminar el salto de línea al final de la línea leída
        linea[strcspn(linea, "\n")] = '\0';
        // Crear una copia de la línea leída para almacenarla en la lista
        char* linea_copia = strdup(linea);
        list_add(instrucciones_del_path, linea);
    }

    fclose(archivo);
*/ //PUEDE SERVIR PARA MEMORIA
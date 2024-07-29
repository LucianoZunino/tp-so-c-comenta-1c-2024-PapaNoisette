#include "acciones_proceso.h"
uint32_t next_pid = 1;
pthread_mutex_t mutex_next_pid;
pthread_mutex_t mutex_NEW;
pthread_mutex_t mutex_EXIT;
pthread_mutex_t socket_memoria_mutex;
//sem_t sem_EXIT;

//char* estado_pcb_desc[6] = {"NEW", "READY", "BLOCKED", "EXIT", "PRIORIDAD", "EXECUTE"};


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

void imprimir_colas(){
    printf("Size NEW: %i\n", list_size(NEW));
    printf("Size READY: %i\n", list_size(READY));
    printf("Size BLOCKED: %i\n", list_size(BLOCKED));
    if (RUNNING != NULL)
        printf("PID RUNNING: %i\n", RUNNING->pid);
}

void crear_proceso(char* path){
    t_pcb *nuevo_pcb = crear_pcb();

    nuevo_pcb->estado = E_NEW;
    pthread_mutex_lock(&mutex_NEW);
    list_add(NEW, nuevo_pcb);
    pthread_mutex_unlock(&mutex_NEW);

   // enviar_proceso_por_paquete(nuevo_pcb, path, fd_memoria, MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS);
    t_buffer* buffer_a_enviar = crear_buffer();
    t_paquete *paquete = crear_paquete(MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS, buffer_a_enviar);
    cargar_int_al_buffer(paquete->buffer, nuevo_pcb->pid);
    cargar_string_al_buffer(paquete->buffer, path);
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);   
    // printf("Crear proceso\n");
    printf("Se inicializaron las estructuras\n");

    log_info(logger_kernel, "Se crea el proceso %i en NEW", nuevo_pcb->pid);
    
    sem_wait(&sem_estructuras_inicializadas);

    sem_post(&sem_NEW);  // no deberia ser despues de recibir el ok de memoria? QUIEN HACE UN SEM_WAIT?


    imprimir_colas();


    imprimir_new();
}

t_pcb *crear_pcb(){

    t_pcb *pcb = malloc(sizeof(t_pcb));

    pthread_mutex_lock(&mutex_next_pid);
    pcb->pid = next_pid;
    next_pid++;
    pthread_mutex_unlock(&mutex_next_pid);

    pcb->program_counter = 0;

    pcb->registros_cpu = malloc(sizeof(t_registros_cpu));
    iniciar_registros(pcb);
    

    pcb->quantum = malloc(sizeof(int));
    
    pcb->quantum = quantum;

    pcb->estado = malloc(sizeof(pcb->estado));



    // ESTA MEMORIA SE ELIMINA EN ELIMINAR_PROCESO

    return pcb;
}

void eliminar_proceso() {
    sem_wait(&sem_EXIT);
    printf("\nAntes de tomar pcb\n\n");
    pthread_mutex_lock(&mutex_EXIT);
    t_pcb* pcb = list_remove(EXIT, 0);
    pthread_mutex_unlock(&mutex_EXIT);
    int pid = pcb->pid;
    printf("\n\n\nANTES DE SOLICITAR LIBERAR MEMORIA\n\n\n");
    solicitar_liberar_en_memoria(pid);
    printf("\n\n\n DESPUES DE SOLICITAR LIBERAR MEMORIA\n\n\n");
    pthread_mutex_lock(&mutex_multiprogramacion);
    grado_actual_multiprogramacion--;
    pthread_mutex_unlock(&mutex_multiprogramacion);
    printf("\n\n\nANTES DE SOLICITAR LIBERAR RECURSOS\n\n\n");
    liberar_recursos_de(pcb);
    printf("\n\n\nDESPUES DE SOLICITAR LIBERAR RECURSOS\n\n\n");
    if(diferencia_de_multiprogramacion <= 0){
        sem_post(&sem_MULTIPROGRAMACION);
    }else{
        diferencia_de_multiprogramacion--;
    }

    log_info(logger_kernel, "Se elimina el proceso %i en EXIT", pid);
    pcb_destruir(pcb);
}

void solicitar_liberar_en_memoria(int pid) {
    t_buffer* buffer = crear_buffer();
    t_paquete* paquete = crear_paquete(LIBERAR_PROCESO_EN_MEMORIA, buffer);
    cargar_int_al_buffer(paquete->buffer, pid);
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);
}

void liberar_recursos_de(t_pcb* pcb) {
    // Recorremos todos los recursos de la lista de recursos disponibles
    printf("\n DENTRO DE LIBERAR_RECURSOS_dE\n\n");
    printf("\nTamaño de lista de recursos_disponibles: %i\n", list_size(recursos_disponibles));

    for(int i = 0; i < list_size(recursos_disponibles); i++){
        // Tomamos un recurso
        t_recurso* recurso = list_get(recursos_disponibles, i);
        printf("\n Despues de tomar recurso\n\n");
        // Sumamos instancias por cada recurso que se le haya asignado al p
        int instancias = 0;
        while(instancias >= 0){
            //sacamos de la lista de asignadoos
            instancias = sumar_instancia(recurso->nombre, pcb);
        }
        printf("\n Despues de while tomar instancias\n\n");
        // Sacamos de la cola de espera
        while(lista_contiene_pcb(recurso->cola_de_espera->elements, pcb)){
            int indice = buscar_index_por_pid(recurso->cola_de_espera->elements, pcb->pid);

            pthread_mutex_lock(&recurso->mutex);
            t_pcb* pcb_a_destruir = list_remove(recurso->cola_de_espera->elements, indice);
            pthread_mutex_unlock(&recurso->mutex);
            pcb_destruir(pcb_a_destruir);
        }
        printf("\n Despues de while cola de espera\n\n");
    }
    printf("\n\n\nFIN LIBERAR_RECURSOS_DE\n\n\n");
}

void mostrar_procesos_por_estado(){
    for(int i = 0; i < list_size(lista_de_estados); i++){
        t_list* lista_actual = list_get(lista_de_estados, i);
        log_info(logger_kernel, "Procesos en estado de %s \n", estado_pcb_desc[i]);
        mostrar_procesos_de(lista_actual);
    }
    log_info(logger_kernel, "Proceso en estado de EXEC \n");
    log_info(logger_kernel, "  - Proceso %i \n", RUNNING->pid);
}

void mostrar_procesos_de(t_list* lista_actual){
    if(list_is_empty(lista_actual)){log_info(logger_kernel, "  No se encuentran procesos en este estado\n");}
    for(int i = 0;i < list_size(lista_actual); i++){
        t_pcb* pcb_actual = list_get(lista_actual, i);
        log_info(logger_kernel, "  - Proceso %i \n", pcb_actual->pid);
    }
}

void iniciar_registros(t_pcb* pcb){
    pcb->registros_cpu->AX = 0;
    pcb->registros_cpu->BX = 0;
    pcb->registros_cpu->CX = 0;
    pcb->registros_cpu->DX = 0;
    pcb->registros_cpu->EAX = 0;
    pcb->registros_cpu->EBX = 0;
    pcb->registros_cpu->ECX = 0;
    pcb->registros_cpu->EDX = 0;
    pcb->registros_cpu->SI = 0;
    pcb->registros_cpu->DI = 0;
}

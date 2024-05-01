#include "acciones_proceso.h"
uint32_t next_pid;
pthread_mutex_t mutex_next_pid;
pthread_mutex_t mutex_NEW;
pthread_mutex_t socket_memoria_mutex;

void crear_proceso(char* path){
    
    FILE* archivo = fopen(path, "r");

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

    t_pcb *nuevo_pcb = crear_pcb(instrucciones_del_path);

    nuevo_pcb->estado = E_NEW;
    
    pthread_mutex_lock(&mutex_NEW);
    list_add(NEW, &nuevo_pcb);
    pthread_mutex_unlock(&mutex_NEW);

    pthread_mutex_lock(&socket_memoria_mutex);
    enviar_memoria_solicitar_inicializar_estructuras(nuevo_pcb, instrucciones_del_path, fd_memoria);

    op_code codigo_operacion = recibir_codigo_operacion(fd_memoria);
    if(codigo_operacion != KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS)
    {
        log_error(logger_kernel, "PID %i - No se pudo inicializar estructuras", nuevo_pcb->pid);
        return;
    }
    recibir_kernel_respuesta_inicializar_estructuras(fd_memoria);
    pthread_mutex_unlock(&socket_memoria_mutex);


    // TODO: Revisar tema del PID para tener el log bien hecho
    log_info(logger_kernel, "Se crea el proceso %i en NEW", nuevo_pcb->pid);

}

t_pcb *crear_pcb(t_list *instrucciones){

     t_pcb *pcb = malloc(sizeof(t_pcb));

    pthread_mutex_lock(&mutex_next_pid);
    pcb->pid = next_pid;
    next_pid++;
    pthread_mutex_unlock(&mutex_next_pid);

    pcb->instrucciones = instrucciones;
    pcb->program_counter = 0;

    t_registros_cpu *registros_cpu = malloc(sizeof(t_registros_cpu));
    memset(registros_cpu->AX, 0, sizeof(registros_cpu->AX));
    memset(registros_cpu->BX, 0, sizeof(registros_cpu->BX));
    memset(registros_cpu->CX, 0, sizeof(registros_cpu->CX));
    memset(registros_cpu->DX, 0, sizeof(registros_cpu->DX));
    memset(registros_cpu->EAX, 0, sizeof(registros_cpu->EAX));
    memset(registros_cpu->EBX, 0, sizeof(registros_cpu->EBX));
    memset(registros_cpu->ECX, 0, sizeof(registros_cpu->ECX));
    memset(registros_cpu->EDX, 0, sizeof(registros_cpu->EDX));
    memset(registros_cpu->SI, 0, sizeof(registros_cpu->SI));
    memset(registros_cpu->DI, 0, sizeof(registros_cpu->DI));

    pcb->registros_cpu = registros_cpu;

    //pcb->quantum = quantum;

    

    return pcb;
}
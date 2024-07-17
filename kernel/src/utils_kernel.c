#include "utils_kernel.h"

t_list* recursos_disponibles;
pthread_mutex_t mutex_recursos_disponibles;
char* estado_pcb_desc[6] = {"NEW", "READY", "BLOCKED", "EXIT", "PRIORIDAD", "EXECUTE"};

void interrumpir_cpu(t_pcb *pcb, motivo_interrupcion motivo){
    enviar_cpu_interrupt(pcb, motivo, fd_cpu_interrupt);
    log_trace(logger_kernel, "Interrumpo la ejecucion de PID %i", pcb->pid);
}

void pcb_destruir(t_pcb *pcb){
    if (pcb != NULL){
        
        free(pcb->registros_cpu);
        free(pcb->quantum);
        free(pcb->estado);
        free(pcb);
    }
}

void enviar_proceso_cpu(t_pcb *pcb, int socket, op_code op_code)
{
    t_buffer* buffer_a_enviar = crear_buffer();
    t_paquete *paquete = crear_paquete(op_code, buffer_a_enviar);
    agregar_pcb(paquete, pcb);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);    
}

int buscar_index_por_pid(t_list* lista, int pid) {
	for (unsigned int i = 0; i < list_size(lista); i++) {
		t_pcb* pcb = list_get(lista, i);
		if (pcb->pid == pid) {
			return i;
		}
	}
	return -1;
}

t_pcb* buscar_pcb_por_pid(int pid){
    unsigned int index;
    t_pcb* pcb;
    if (index = buscar_index_por_pid(NEW, pid) != -1){
        pthread_mutex_lock(&mutex_NEW);
        pcb = list_remove(NEW, index);
        pthread_mutex_unlock(&mutex_NEW);
    } else if (index = buscar_index_por_pid(READY, pid) != -1){ 
        pthread_mutex_lock(&mutex_READY);
        pcb = (t_pcb*) list_remove(READY, index);
        pthread_mutex_unlock(&mutex_READY);
    } else if (index = buscar_index_por_pid(BLOCKED, pid) != -1){ 
        pthread_mutex_lock(&mutex_BLOCKED);
        pcb = list_remove(BLOCKED, index);
        pthread_mutex_unlock(&mutex_BLOCKED);
    } else if (index = buscar_index_por_pid(PRIORIDAD, pid) != -1){ 
        pthread_mutex_lock(&mutex_PRIORIDAD);
        pcb = list_remove(PRIORIDAD, index);
        pthread_mutex_unlock(&mutex_PRIORIDAD);
    }
    return pcb;

}

void pasar_proceso_a_exit(int pid){
    t_pcb* pcb = buscar_pcb_por_pid(pid);
    pcb->estado = E_EXIT;
    
    pthread_mutex_lock(&mutex_EXIT);
	list_add(EXIT, pcb);
	pthread_mutex_unlock(&mutex_EXIT);

    sem_post(&sem_EXIT);
}

int buscar_recurso(char* recurso){
    for (unsigned int i = 0; i < list_size(recursos); i++){
        if (string_equals_ignore_case(list_get(recursos, i), recurso))
            return i;
    }
    return -1;
}

void enviar_a_exit(t_pcb* pcb){
    cambio_de_estado(pcb, E_EXIT);
    //cb->estado = E_EXIT;
	pthread_mutex_lock(&mutex_EXIT);
	list_add(EXIT, pcb);
	pthread_mutex_unlock(&mutex_EXIT);
    sem_post(&sem_EXIT); //agregue este semaforo
}


void restar_instancia(char* nombre_recurso, t_pcb *pcb){
    int index = buscar_recurso(nombre_recurso);
    t_recurso* recurso = list_get(recursos_disponibles, index);

    if (recurso->instancias <= 0){ //caso en el que no hay recurso de instancia disponible
        pthread_mutex_lock(&mutex_BLOCKED);
	    list_add(BLOCKED, pcb);
	    pthread_mutex_unlock(&mutex_BLOCKED);
        cambio_de_estado(pcb, E_BLOCKED); //FALTABA CAMBIAR EL ESTADO

        pthread_mutex_lock(&recurso->mutex);
        list_add(recurso->cola_de_espera, pcb);
        pthread_mutex_unlock(&recurso->mutex);
    } else { //caso en el que hay recurso de instancia disponible
        pthread_mutex_lock(&recurso->mutex);
        recurso->instancias =- 1;
        pthread_mutex_unlock(&recurso->mutex);
        
        pthread_mutex_lock(&recurso->mutex);
        list_add(recurso->pcb_asignados, pcb);
        pthread_mutex_unlock(&recurso->mutex);
        
        if(pcb->quantum != quantum){ // CUANDO SE UTILIZA VRR, SE CONTEMPLA EL CASO DE PRIORIDAD POR QUANTUM
            pthread_mutex_lock(&mutex_PRIORIDAD);
			list_add(PRIORIDAD, pcb);
			pthread_mutex_unlock(&mutex_PRIORIDAD);
            cambio_de_estado(pcb, E_PRIORIDAD);
        }else {
            pthread_mutex_lock(&mutex_READY);
			list_add(READY, pcb);
			pthread_mutex_unlock(&mutex_READY);
            cambio_de_estado(pcb, E_READY);
        }
        
        list_remove(recursos_disponibles, index); //se actualiza la lista de recursos_disponibles
        list_add_in_index(recursos_disponibles, index, &recurso);

    }
}

int sumar_instancia(char* nombre_recurso, t_pcb* pcb){
    int index = buscar_recurso(nombre_recurso);
    t_recurso* recurso = list_get(recursos_disponibles, index);

    if(lista_contiene_pcb(recurso->pcb_asignados, pcb)){
        return -1;
    }
    
    if(recurso->instancias >= 0){
        pthread_mutex_lock(&recurso->mutex);
        recurso->instancias =- 1;
        pthread_mutex_unlock(&recurso->mutex);
        
        if(!queue_is_empty(recurso->cola_de_espera)){ // Verifico que haya procesos esperando el recurso

            t_pcb* nuevo_pcb = queue_pop(recurso->cola_de_espera); // Tomo el primero
            
            if(pcb->estado != E_EXIT){
                if(pcb->quantum != quantum){ // Devuelvo el proceso original a la cola de ready o de prioridad, segun corresponda
                    pthread_mutex_lock(&mutex_PRIORIDAD);
			        list_add(PRIORIDAD, pcb);
			        pthread_mutex_unlock(&mutex_PRIORIDAD);
                    cambio_de_estado(pcb, E_PRIORIDAD);
                }else {
                    pthread_mutex_lock(&mutex_READY);
			        list_add(READY, pcb);
			        pthread_mutex_unlock(&mutex_READY);
                    cambio_de_estado(pcb, E_READY);
                }
            }
            // char* nombre = recurso.nombre;
            restar_instancia(recurso->nombre, nuevo_pcb); // Llamo a la funcion para asignarle las instancia al nuevo proceso

            // Sacamos el pcb de la lista de asignados
            pthread_mutex_lock(&recurso->mutex);
            int indice_pcb = buscar_index_por_pid(recurso->pcb_asignados, pcb->pid);
            if (indice_pcb == (-1)){ 
                log_error(logger_kernel, "NO SE ENCUENTRA EN PCB_ASIGNADOS");
            }
            list_remove(recurso->pcb_asignados, indice_pcb);
            pthread_mutex_unlock(&recurso->mutex);
            
            //
            int index_pcb = buscar_index_por_pid(BLOCKED, nuevo_pcb->pid);
            pthread_mutex_lock(&mutex_BLOCKED);
            list_remove(BLOCKED, index_pcb);  // Elimino el nuevo proceso de la lista de bloqueados 
            pthread_mutex_unlock(&mutex_BLOCKED);


        }
    }

    pthread_mutex_lock(&mutex_recursos_disponibles);
    list_remove(recursos_disponibles, index);
    list_add_in_index(recursos_disponibles, index, &recurso);
    pthread_mutex_unlock(&mutex_recursos_disponibles);
    return 1;
    }

int buscar_interfaz(char* nombre) {
    t_interfaz* interfaz;
    for(int i = 0; i<list_size(interfaces); i++){
        interfaz = list_get(interfaces, i);
        if (string_equals_ignore_case(interfaz->nombre, nombre)) {
            return i;
        }  
    }
    return NULL;
}

bool lista_contiene_pcb(t_list* lista, t_pcb* pcb){
    for(int i = 0; i < list_size(lista); i ++){
        t_pcb* aux;
        if(aux->pid == pcb->pid){return true;}
    }
    return false;
}

void cambio_de_estado(t_pcb* pcb, int estado_nuevo){
    int estado = pcb->estado;
    char* string_estado_anterior = estado_pcb_desc[estado];
    char* string_estado_actual = estado_pcb_desc[estado_nuevo];
    log_info("PID: <%i> - Estado Anterior: <%s> - Estado Actual: <%s>", pcb->pid, string_estado_anterior, string_estado_actual );
    pcb->estado = estado_nuevo;
}
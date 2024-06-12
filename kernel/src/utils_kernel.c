#include "utils_kernel.h"

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
    pcb->estado = E_EXIT;
	pthread_mutex_lock(&mutex_EXIT);
	list_add(EXIT, pcb);
	pthread_mutex_unlock(&mutex_EXIT);
}

void restar_instancia(char* recurso){
    int index = buscar_recurso(recurso);
    int instancias = list_get(instancias_recursos, index);
    if (instancias < 0){
        // Bloquear proceso
    } else {
        // Asignar instancia
        // Restar instancia
    }

    sem_wait(contador_recursos[index]);

}
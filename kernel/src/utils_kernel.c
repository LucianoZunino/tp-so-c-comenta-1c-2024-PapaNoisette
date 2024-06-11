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
#include "mensajes.h"





/// @brief Recibe el codigo de operacion del socket
/// @note Se debe utilizar antes de cualquier llamado a otros recibir_...()
/// @param socket
/// @return
op_code recibir_codigo_operacion(int socket)
{
    op_code codigo_operacion;
    recv(socket, &codigo_operacion, sizeof(codigo_operacion), MSG_WAITALL);
    return codigo_operacion;
}
/// @brief Envia el pcb, path a archivo de pseudocodigo  por motivo de INICIALIZAR_ESTRUCTURAS al socket
/// @param pcb
/// @param archivo_pseudocodigo

/// @param socket
/*void enviar_memoria_solicitar_inicializar_estructuras(t_pcb *pcb, t_list *archivo_pseudocodigo, int socket)
{
    t_buffer* buffer_a_enviar = crear_buffer();
    t_paquete *paquete = crear_paquete(MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS, buffer_a_enviar);
    agregar_pcb(paquete, pcb);
    agregar_intrucciones(paquete, archivo_pseudocodigo);
    //agregar_a_paquete(paquete, &tamanio, sizeof(tamanio));
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}
*/
void agregar_pcb (t_paquete *paquete, t_pcb *pcb)
{
    //cargar_datos_al_buffer(paquete->buffer, pcb->pid, sizeof(int));
    
    cargar_int_al_buffer(paquete->buffer, pcb->pid);
    //cargar_datos_al_buffer(paquete->buffer, pcb->instrucciones, sizeof(pcb->instrucciones));
    cargar_int_al_buffer(paquete->buffer, pcb->program_counter);
    cargar_datos_al_buffer(paquete->buffer, pcb->registros_cpu, sizeof(pcb->registros_cpu));
    cargar_int_al_buffer(paquete->buffer, pcb->quantum);
    cargar_datos_al_buffer(paquete->buffer, &pcb->estado, sizeof(pcb->estado));
}

/*/// @brief Agrega longitud y la intrucciones al buffer del paquete
/// @param paquete
/// @param lista
void agregar_intrucciones(t_paquete *paquete, t_list* lista)
{   if(lista != NULL){
    int longitud = list_size(lista);
    char* cadena[100];
    int i = 0;
    while( i != longitud){
        cadena[i] = list_get(lista, i);
        i++;
    }

    cargar_int_al_buffer(paquete->buffer, longitud);
    cargar_datos_al_buffer(paquete->buffer, cadena, sizeof(char) * longitud);
    }
}   // PUEDE SERVIR PARA MEMORIA AL MOMENTO DE SACAR LAS INSTRUCCIONES DEL PATH
*/


void recibir_ok(int socket)
{
    int _;
    recv(socket, &_, sizeof(_), MSG_WAITALL);
    return;
}

/// @brief Recibe validacion de inicializar estructuras
/// @param pcb
/// @param socket
void recibir_kernel_respuesta_inicializar_estructuras(int socket)
{
    recibir_ok(socket);
}

/// @brief envia proceso a el corto plazo a cpu
/// @param pcb
/// @param socket
void enviar_proceso_por_paquete(t_pcb *pcb, char *archivo_pseudocodigo, int socket, op_code op_code)
{
    t_buffer* buffer_a_enviar = crear_buffer();
    t_paquete *paquete = crear_paquete(op_code, buffer_a_enviar);
    agregar_pcb(paquete, pcb);
    cargar_string_al_buffer(paquete->buffer, archivo_pseudocodigo);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);    
}



/// @brief Envia CPU_INTERRUPT y el pcb al socket indicado
/// @param pcb
/// @param socket
void enviar_cpu_interrupt(t_pcb *pcb, motivo_interrupcion motivo, int socket){   
    t_buffer* buffer_a_enviar = crear_buffer();
    t_paquete *paquete = crear_paquete(CPU_INTERRUPT, buffer_a_enviar);
    agregar_pcb(paquete, pcb);
    cargar_datos_al_buffer(paquete->buffer, &motivo, sizeof(motivo));
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}



/// @brief Envia al kernel el pcb por motivo de interrupcion
/// @param pcb
/// @param socket
void enviar_kernel_interrupt(t_pcb *pcb, motivo_interrupcion motivo, int socket)
{
  

}


void enviar_memoria_solicitar_instruccion(t_pcb *pcb, int socket)
{
  
}

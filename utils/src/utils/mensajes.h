
#ifndef MENSAJES_H_
#define MENSAJES_H_
#include "utils.h"



op_code recibir_codigo_operacion(int socket);
void enviar_proceso_por_paquete(t_pcb *pcb,char *archivo_pseudocodigo, int socket, op_code op_code);
//void agregar_intrucciones(t_paquete *paquete, t_list* lista);
void recibir_ok(int socket);
void recibir_kernel_respuesta_inicializar_estructuras(int socket);

void enviar_proceso_a_cpu(t_pcb *pcb, int socket);
void enviar_cpu_interrupt(t_pcb *pcb, motivo_interrupcion motivo, int socket);
//void enviar_memoria_solicitar_instruccion(t_pcb *pcb, int socket);
void enviar_memoria_solicitar_instruccion(int pid,int program_counter, int socket);
void enviar_memoria_solicitar_instruccion2(t_pcb *pcb, int socket);
void enviar_instruccion(char* instruccion, int socket);
void agregar_pcb (t_paquete *paquete, t_pcb *pcb);


#endif 
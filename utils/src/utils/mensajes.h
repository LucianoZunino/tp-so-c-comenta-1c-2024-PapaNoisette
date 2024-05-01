
#ifndef MENSAJES_H_
#define MENSAJES_H_
#include "utils.h"

void validar_buffer(int socket);

op_code recibir_codigo_operacion(int socket);
void enviar_memoria_solicitar_inicializar_estructuras(t_pcb *pcb, t_list *archivo_pseudocodigo, int socket);
void agregar_intrucciones(t_paquete *paquete, t_list* lista);
void recibir_ok(int socket);
void recibir_kernel_respuesta_inicializar_estructuras(int socket);

#endif 
#ifndef ACCIONES_PROCESO
#define ACCIONES_PROCESO

#include "main.h"
#include <utils/utils.h>
#include <readline/readline.h>

//extern char** comando_consola_desc;


void crear_proceso(char* path);
t_pcb* crear_pcb();
void eliminar_proceso();
void iniciar_registros(t_pcb* pcb);
void mostrar_procesos_por_estado();
void mostrar_procesos_de(t_list* lista_actual);
void liberar_recursos_de(t_pcb* pcb);
void solicitar_liberar_en_memoria(int pid);


#endif
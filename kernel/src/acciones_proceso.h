#ifndef ACCIONES_PROCESO
#define ACCIONES_PROCESO

#include "main.h"
#include <readline/readline.h>

//extern char** comando_consola_desc;


void crear_proceso(char* path);
t_pcb* crear_pcb(t_list *instrucciones);


#endif
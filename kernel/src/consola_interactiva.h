#ifndef CONSOLA_INTERACTIVA
#define CONSOLA_INTERACTIVA

#include "main.h"
#include <readline/readline.h>

void iniciar_consola_interactiva();
bool validacion_de_instruccion_de_consola(char* leido);
void ejecutar_instruccion(char* leido);

#endif
#ifndef CONSOLA_INTERACTIVA
#define CONSOLA_INTERACTIVA

#include "main.h"
#include <readline/readline.h>

void iniciar_consola_interactiva();
bool validacion_de_instruccion_de_consola(char* leido);
comando_consola validacion_de_instruccion_de_consola1(char* leido, bool* comando_valido);
void ejecutar_instruccion1(char* leido, comando_consola comando);
void ejecutar_instruccion(char* leido);

#endif
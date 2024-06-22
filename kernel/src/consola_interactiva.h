#ifndef CONSOLA_INTERACTIVA
#define CONSOLA_INTERACTIVA

#include "main.h"
#include <string.h>
#include <readline/readline.h>

//extern char** comando_consola_desc;


void iniciar_consola_interactiva();
comando_consola validar_entrada(char* codigo);
void ejecutar_instruccion(char** leido, comando_consola comando);
void abrir_archivo(char** comandos[], char* path, int* j);

#endif
#ifndef CPU_CICLO_INSTRUCCIONES_H
#define CPU_CICLO_INSTRUCCIONES_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <utils/utils.h>
#include <main.h>
#include "utils.h"
#include "main.h"
typedef enum {
    SET,  
    MOV_IN, 
    MOV_OUT,
    SUM, 
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT
} t_instruction_code;

int ciclo_de_instruccion(t_pcb *pcb);
void devolver_contexto_ejecucion(t_pcb *pcb);//recibe pid?

//void imprimir_contexto_actual();

#endif

#ifndef CPU_UTILS_H
#define CPU_UTILS_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <utils/utils.h>

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
} ex_instruction;

/*SET SET 
MOV_IN 
MOV_OUT
SUM 
SUB
INZ
RESIZE
COPY_STRING
WAIT
SIGNAL
IO_GEN_SLEEP
IO_STDIN_READ
IO_STDOUT_WRITE
IO_FS_CREATE
IO_FS_DELETE
IO_FS_TRUNCATE
IO_FS_WRITE
IO_FS_READ
EXIT*/

//void cambiar_contexto(void* buffer);   // Setea los registros y demas estructuras del CPU con los datos recibidos de un proceso
//ex_instruction decode(t_instruccion* instruccion); 
ex_instruction decode(char* instruccion);// Toma una instruccion y devuelve a que elemento del enum as_instruction corresponde
//cod_op_kernel ejecutar_instrucciones(); // Ejecuta instrucciones hasta  que se encuentra con alguna que requiere terminar la ejecucion (break)
//void imprimir_contexto_actual();

#endif

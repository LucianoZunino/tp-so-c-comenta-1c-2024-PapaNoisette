#include "utils.h"

uint8_t AX;
uint8_t BX;
uint8_t CX;
uint8_t DX;
uint32_t EAX;
uint32_t EBX;
uint32_t ECX;
uint32_t EDX;
uint32_t SI;
uint32_t DI;


void loggear_ejecucion(t_instruccion *instruccion)//log obligatorio
{
    log_info(logger_cpu, "Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s",
             instruccion->instruccion,
             instruccion->arg1,
             instruccion->arg2,
             instruccion->arg3);
}

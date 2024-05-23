#include "utils.h"

void loggear_ejecucion(t_instruccion *instruccion)//log obligatorio
{
    log_info(logger_cpu, "Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s",
             instruccion->instruccion,
             instruccion->arg1,
             instruccion->arg2,
             instruccion->arg3);
}

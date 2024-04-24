#include "utils.h"

void loggear_ejecucion(t_instruccion *instruccion)
{
    log_info(logger_cpu, "PID: %d - Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s", PID_RUNNING,
             instruccion->instruccion,
             instruccion->arg1,
             instruccion->arg2,
             instruccion->arg3);
}
ex_instruction decode(char *instruccion)
{

#include "utils.h"

ex_instruction decode(char *instruccion)
    {
        if (strcmp(instruccion, "SET") == 0)
        {
            return SET;
        }
        else if (strcmp(instruccion, "MOV_IN") == 0)
        {
            return MOV_IN;
        }
        else if (strcmp(instruccion, "MOV_OUT") == 0)
        {
            return MOV_OUT;
        }
        else if (strcmp(instruccion, "SUM") == 0)
        {
            return SUM;
        }
        else if (strcmp(instruccion, "SUB") == 0)
        {
            return SUB;
        }
        else if (strcmp(instruccion, "INZ") == 0)
        {
            return INZ;
        }
        else if (strcmp(instruccion, "RESIZE") == 0)
        {
            return RESIZE;
        
        else if (strcmp(instruccion, "COPY_STRING") == 0)
        {
            return COPY_STRING;
        }
        else if (strcmp(instruccion, "WAIT") == 0)
        {
            return WAIT;
        }
        else if (strcmp(instruccion, "SIGNAL") == 0)
        {
            return SIGNAL;
        }
        else if (strcmp(instruccion, "IO_GEN_SLEEP") == 0)
        {
            return IO_GEN_SLEEP;
        }
        else if (strcmp(instruccion, "IO_STDIN_READ") == 0)
        {
            return IO_STDIN_READ;
        }
        else if (strcmp(instruccion, "IO_STDOUT_WRITE") == 0)
        {
            return IO_STDOUT_WRITE;
        }
        else if (strcmp(instruccion, "IO_FS_CREATE") == 0)
        {
            return IO_FS_CREATE;
        }
        else if (strcmp(instruccion, "IO_FS_DELETE") == 0)
        {
            return IO_FS_DELETE;
        }
        else if (strcmp(instruccion, "IO_FS_TRUNCATE") == 0)
        {
            return IO_FS_TRUNCATE;
        }
        else if (strcmp(instruccion, "IO_FS_WRITE") == 0)
        {
            return IO_FS_WRITE;
        }
        else if (strcmp(instruccion, "IO_FS_READ") == 0)
        {
            return IO_FS_READ;
        }
        else if (strcmp(instruccion, "EXIT") == 0)
        {
            return EXIT;
        }
        else
        {
            log_error(logger_cpu, "La intrucción no es válida");
            return -1; // supuestamente las instrucciones nunca vienen incorrectas
        }
    }

void ejecutar_set(t_instruccion * instruccion)
    {
        if (strcmp(instruccion->arg1, "AX") == 0)
        {
            strncpy(AX, instruccion->arg2, 4);
        }
        else if (strcmp(instruccion->arg1, "BX") == 0)
        {
            strncpy(BX, instruccion->arg2, 4);
        }
        else if (strcmp(instruccion->arg1, "CX") == 0)
        {
            strncpy(CX, instruccion->arg2, 4);
        }
        else if (strcmp(instruccion->arg1, "DX") == 0)
        {
            strncpy(DX, instruccion->arg2, 4);
        }
        else if (strcmp(instruccion->arg1, "EAX") == 0)
        {
            strncpy(EAX, instruccion->arg2, 32);
        }
        else if (strcmp(instruccion->arg1, "EBX") == 0)
        {
            strncpy(EBX, instruccion->arg2, 32);
        }
        else if (strcmp(instruccion->arg1, "ECX") == 0)
        {
            strncpy(ECX, instruccion->arg2, 32);
        }
        else if (strcmp(instruccion->arg1, "EDX") == 0)
        {
            strncpy(EDX, instruccion->arg2, 32);
        }
        else if (strcmp(instruccion->arg1, "SI") == 0)
        {
            strncpy(SI, instruccion->arg2, 32);
        }
        else if (strcmp(instruccion->arg1, "DI") == 0)
        {
            strncpy(DI, instruccion->arg2, 32);
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SET es incorrecto");
        }
    }

cod_op_kernel ejecutar_instrucciones()
    {

        while (true)
        {
            t_instruccion *instruccion = list_get(INSTRUCTION_LIST, PROGRAM_COUNTER);
            PROGRAM_COUNTER += 1;
            as_instruction instruction_code = decode(instruccion);

            loggear_ejecucion(instruccion);

            switch (instruction_code)
            {
            case SET:
                ejecutar_set(instruccion);
                break;
            case MOV_IN:
                break;
            case MOV_OUT:
                break;
            case SUM:
                break;
            case SUB:
                break;
            case JNZ:
                break;
            case RESIZE:
                break;
            case COPY_STRING:
                break;
            case WAIT:
                break;
            case SIGNAL:
                break;
            case IO_GEN_SLEEP:
                break;
            case IO_STDIN_READ:
                break;
            case IO_STDOUT_WRITE:
                break;
            case IO_FS_CREATE:
                break;
            case IO_FS_DELETE:
                break;
            case IO_FS_TRUNCATE:
                break;
            case IO_FS_WRITE:
                break;
            case IO_FS_READ:
                break;
            case EXIT:
                break;
            default:
                break;
            }

            // imprimir_contexto_actual();

            if (PROGRAM_COUNTER >= list_size(INSTRUCTION_LIST))
            {
                return CPU_EXIT; // o hacer otra cosa para manejar este error
            }
        }
    }
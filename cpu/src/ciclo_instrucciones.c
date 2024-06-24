
#include "instrucciones.h"

#define SIN_INTERRUPCION -1
#define CONTINUAR_CICLO 0
#define SALIR_DE_CICLO 1

int pid_interrupcion = SIN_INTERRUPCION;
motivo_interrupcion motivo;
pthread_mutex_t interrupcion_mutex;

/// @brief Solicita a memoria la siguiente instruccion
/// @note
/// @param t_pcb
/// @return t_instruccion
t_instruccion *fetch()
{
    log_info(logger_cpu, "Ejecutando FETCH PID: %i Program Counter: %i", EXEC->pid, EXEC->program_counter);
    // enviar_memoria_solicitar_instruccion(proceso, fd_memoria); CPU_SOLICITA_INSTRUCCION

    // op_code codigo_operacion =recibir_codigo_operacion(fd_memoria);
    char *cpu_respuesta_instruccion; // la instruccion sin parsear tipo IO_FS_READ Int4 notas.txt BX ECX EDX

    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    // esta parte iria en el hilo
    /* if (codigo_operacion==MEMORIA_ENVIA_INSTRUCCION){
     char *cpu_respuesta_instruccion  = recibir_cadena(fd_memoria);
     }
     else
     {
     log_error(logger_cpu, "Se recibio un OP_CODE distinto de MEMORIA_ENVIA_INSTRUCCION");
     //aca no se si meter un while o ke...
     }*/

    if (sscanf(cpu_respuesta_instruccion, "%s %s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3) == 4)
    {
        // se cargaron los 4 strings
    }
    else if (sscanf(cpu_respuesta_instruccion, "%s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2) == 3)
    {
        strcpy(instruccion->arg3, "");
    }
    else if (sscanf(cpu_respuesta_instruccion, "%s %s", instruccion->instruccion, instruccion->arg1) == 2)
    {
        strcpy(instruccion->arg2, "");
        strcpy(instruccion->arg3, "");
    }
    else if (sscanf(cpu_respuesta_instruccion, "%s", instruccion->instruccion) == 1)
    {
        strcpy(instruccion->arg1, "");
        strcpy(instruccion->arg2, "");
        strcpy(instruccion->arg3, "");
    }
    printf("Devuelve la instruccion\n");
    return instruccion;
}

/// @brief Procesa una instruccion para un proceso
/// @note Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar y si la misma requiere de una traducción de dirección lógica a dirección física.
/// @param t_instruccion, t_pcb
/// @return retorna un flag para sabes si sigue el ciclo o se interrumpe
int decode_excute(t_instruccion *instruccion)
{
    EXEC->program_counter += 1; // lo hago aca para que en tal caso el jnz lo sobreescriba
    loggear_ejecucion(instruccion);
    if (strcmp(instruccion->instruccion, "SET") == 0)
    {
        ejecutar_set(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "MOV_IN") == 0)
    {
        ejecutar_mov_in(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "MOV_OUT") == 0)
    {
        ejecutar_mov_out(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "SUM") == 0)
    {
        ejecutar_sum(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "SUB") == 0)
    {
        ejecutar_sub(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "JNZ") == 0)
    {
        ejecutar_jnz(instruccion->arg1, instruccion->arg2);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "RESIZE") == 0)
    {
        if (ejecutar_resize(instruccion->arg1) == 0)
        {
            return CONTINUAR_CICLO;
        }
        else
        {
            return SALIR_DE_CICLO;
        }
    }
    else if (strcmp(instruccion->instruccion, "COPY_STRING") == 0)
    {
        ejecutar_copy_string(instruccion->arg1);
        return CONTINUAR_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "WAIT") == 0)
    {
        ejecutar_wait(instruccion->arg1);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "SIGNAL") == 0)
    {
        ejecutar_signal(instruccion->arg1);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_GEN_SLEEP") == 0)
    {
        ejecutar_io_gen_sleep(instruccion->arg1, instruccion->arg2);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_STDIN_READ") == 0)
    {
        ejecutar_io_stdin_read(instruccion->arg1, instruccion->arg2, instruccion->arg3);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_STDOUT_WRITE") == 0)
    {
        ejecutar_io_stdout_write(instruccion->arg1, instruccion->arg2, instruccion->arg3);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_FS_CREATE") == 0)
    {
        ejecutar_io_fs_create(instruccion->arg1, instruccion->arg2);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_FS_DELETE") == 0)
    {
        ejecutar_io_fs_delete(instruccion->arg1, instruccion->arg2);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_FS_TRUNCATE") == 0)
    {
        ejecutar_io_fs_truncate(instruccion->arg1, instruccion->arg2, instruccion->arg3);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_FS_WRITE") == 0)
    {
        ejecutar_io_fs_write(instruccion->arg1, instruccion->arg2, instruccion->arg3, instruccion->arg4, instruccion->arg5);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "IO_FS_READ") == 0)
    {
        ejecutar_io_fs_read(instruccion->arg1, instruccion->arg2, instruccion->arg3, instruccion->arg4, instruccion->arg5);
        return SALIR_DE_CICLO;
    }
    else if (strcmp(instruccion->instruccion, "EXIT") == 0)
    {
        ejecutar_exit();
        return SALIR_DE_CICLO;
    }
    else
    {
        log_error(logger_cpu, "La intrucción no es válida");
        return -1; // supuestamente las instrucciones nunca vienen incorrectas, se deja para pruebas
    }
}

int ciclo_de_instruccion()
{
    // Recibo instruccion y la mando a ejecutar
    printf(">>>>>Entro a ciclo_de_instruccion  \n");
    int estado_ciclo = 0;                                     // 0 continuar 1 salir de ciclo
    t_instruccion *instruccion = malloc(sizeof(instruccion)); // probar si realmente hace falta malloquearla
    instruccion = fetch();

    estado_ciclo = decode_excute(instruccion); // lo h

    free(instruccion->instruccion);
    free(instruccion);

    // Chequeo si la instruccion recibida no fue un EXIT
    // por ahora lo comento calculo que lo trato en decode_execute
    //   if(estado_ciclo == 1)return 1;

    // Chequeo interrupciones

    pthread_mutex_lock(&interrupcion_mutex);
    if (pid_interrupcion == EXEC->pid)
    {
        estado_ciclo = SALIR_DE_CICLO;
        pid_interrupcion = SIN_INTERRUPCION;
        pthread_mutex_unlock(&interrupcion_mutex);
        motivo_interrupcion motivo = motivo;
        enviar_kernel_interrupt(EXEC,motivo, fd_kernel_dispatch);
    }
    else
    {
        if (pid_interrupcion != SIN_INTERRUPCION)
            log_debug(logger_cpu, "Hubo interrupcion pero para otro pid. PID en ejeucion: %i - PID de interrupcion: %i", EXEC->pid, pid_interrupcion);
        pthread_mutex_unlock(&interrupcion_mutex);
    }

    return estado_ciclo;
}

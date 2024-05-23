
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
t_instruccion *fetch(t_pcb *pcb)//a esta altura ya tendria el pcb->pid
{
    log_info(logger_cpu, "Ejecutando FETCH PID: %i Program Counter: %i", pcb->pid, pcb->program_counter);
    enviar_memoria_solicitar_instruccion(pcb, fd_memoria);
    
    op_code codigo_operacion =recibir_codigo_operacion(fd_memoria);
    char *cpu_respuesta_instruccion ;//la instruccion sin parsear tipo IO_FS_READ Int4 notas.txt BX ECX EDX

    t_instruccion * instruccion =malloc(sizeof(t_instruccion));

    if (codigo_operacion==MEMORIA_ENVIA_INSTRUCCION){
    char *cpu_respuesta_instruccion  = recibir_cadena(fd_memoria);
    }
    else 
    {
    log_error(logger_cpu, "Se recibio un OP_CODE distinto de MEMORIA_ENVIA_INSTRUCCION");
    //aca no se si meter un while o ke...
    }

//pattern matchin?
sscanf(cpu_respuesta_instruccion, "%100s", instruccion->instruccion);
sscanf(cpu_respuesta_instruccion, "%100s %100s ", instruccion->instruccion, instruccion->arg1);
sscanf(cpu_respuesta_instruccion, "%100s %100s %100s", instruccion->instruccion, instruccion->arg1, instruccion->arg2);
sscanf(cpu_respuesta_instruccion, "%100s %100s %100s %100s", instruccion->instruccion,instruccion->arg1, instruccion->arg2, instruccion->arg3);
sscanf(cpu_respuesta_instruccion, "%100s %100s %100s %100s %100s", instruccion->instruccion,instruccion->arg1, instruccion->arg2, instruccion->arg3,instruccion->arg4);
sscanf(cpu_respuesta_instruccion, "%100s %100s %100s %100s %100s %100s", instruccion->instruccion,instruccion->arg1, instruccion->arg2, instruccion->arg3,instruccion->arg4,instruccion->arg5);

    return instruccion;
}





/// @brief Procesa una instruccion para un proceso  
/// @note Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar y si la misma requiere de una traducción de dirección lógica a dirección física.
/// @param t_instruccion, t_pcb
/// @return retorna un flag para sabes si sigue el ciclo o se interrumple
int decode_excute(t_instruccion * instruccion,t_pcb *pcb){

        PROGRAM_COUNTER += 1; //lo hago aca para que en tal caso el jnz lo sobreescriba
        loggear_ejecucion(instruccion);

        if (strcmp(instruccion->instruccion, "SET") == 0)
        {
                ejecutar_set(instruccion->arg1,instruccion->arg2);
                return CONTINUAR_CICLO;

        }
        else if (strcmp(instruccion->instruccion, "MOV_IN") == 0)
        {
            return MOV_IN;
        }
        else if (strcmp(instruccion->instruccion, "MOV_OUT") == 0)
        {
            return MOV_OUT;
        }
        else if (strcmp(instruccion->instruccion, "SUM") == 0)
        {
                ejecutar_sum(instruccion->arg1,instruccion->arg2);
                return CONTINUAR_CICLO;

        }
        else if (strcmp(instruccion->instruccion, "SUB") == 0)
        {
                ejecutar_sub(instruccion->arg1,instruccion->arg2);
                return CONTINUAR_CICLO;

        }
        else if (strcmp(instruccion->instruccion, "JNZ") == 0)
        {
                ejecutar_jnz(instruccion->arg1,instruccion->arg2);
                return CONTINUAR_CICLO;

        }
        else if (strcmp(instruccion->instruccion, "RESIZE") == 0)
        {
            return RESIZE;
        }
        else if (strcmp(instruccion->instruccion, "COPY_STRING") == 0)
        {
            return COPY_STRING;
        }
        else if (strcmp(instruccion->instruccion, "WAIT") == 0)
        {
            return WAIT;
        }
        else if (strcmp(instruccion->instruccion, "SIGNAL") == 0)
        {
            return SIGNAL;
        }
        else if (strcmp(instruccion->instruccion, "IO_GEN_SLEEP") == 0)
        {
                ejecutar_io_gen_sleep(instruccion->arg1,instruccion->arg2);
                return SALIR_DE_CICLO;//autodesalojo??

        }
        else if (strcmp(instruccion->instruccion, "IO_STDIN_READ") == 0)
        {
            return IO_STDIN_READ;
        }
        else if (strcmp(instruccion->instruccion, "IO_STDOUT_WRITE") == 0)
        {
            return IO_STDOUT_WRITE;
        }
        else if (strcmp(instruccion->instruccion, "IO_FS_CREATE") == 0)
        {
            return IO_FS_CREATE;
        }
        else if (strcmp(instruccion->instruccion, "IO_FS_DELETE") == 0)
        {
            return IO_FS_DELETE;
        }
        else if (strcmp(instruccion->instruccion, "IO_FS_TRUNCATE") == 0)
        {
            return IO_FS_TRUNCATE;
        }
        else if (strcmp(instruccion->instruccion, "IO_FS_WRITE") == 0)
        {
            return IO_FS_WRITE;
        }
        else if (strcmp(instruccion->instruccion, "IO_FS_READ") == 0)
        {
            return IO_FS_READ;
        }
        else if (strcmp(instruccion->instruccion, "EXIT") == 0)
        {

                devolver_contexto_ejecucion(pcb);
                //finalizar_proceso(pcb);//hacerrr
                return SALIR_DE_CICLO;

        }
        else
        {
            log_error(logger_cpu, "La intrucción no es válida");
            return -1; // supuestamente las instrucciones nunca vienen incorrectas
        }
    }


//falta la funcion que deserializa y ejecuta ,primero en kernel, luego en io 
void devolver_contexto_ejecucion(t_pcb *pcb)
{

    log_info(logger_cpu, " ENVIANDO CONTEXTO DE EJECUCION A KERNEL");
    //void imprimir_contexto_actual();

    t_buffer* buffer_a_enviar = crear_buffer();


     
     //motivo desalojo
     t_paquete* paquete = crear_paquete(CONTEXTO_EJECUCION, buffer_a_enviar);
     enviar_paquete(paquete, fd_kernel_dispatch);
     eliminar_paquete(paquete);
}
     
/*
void recibir_contexto_ejecucion(){//mover a kernel!!
*
}*/



int ciclo_de_instruccion(t_pcb *pcb)
{
    t_instruccion *instruccion =malloc(sizeof(instruccion));// probar si realmente hace falta malloquearla
    instruccion = fetch(pcb);

    int seguir  = decode_excute(instruccion,pcb);//lo h

    free(instruccion->instruccion);
    free(instruccion);

    if(seguir == SALIR_DE_CICLO)
        return SALIR_DE_CICLO;

    pthread_mutex_lock(&interrupcion_mutex);
    if(pid_interrupcion == pcb->pid)
    {
        seguir = SALIR_DE_CICLO;
        pid_interrupcion = SIN_INTERRUPCION;
        motivo_interrupcion motivo_pcb = motivo;
        pthread_mutex_unlock(&interrupcion_mutex);

        enviar_kernel_interrupt(pcb, motivo_pcb, fd_kernel_dispatch);
    }
    else
    {
        if(pid_interrupcion != SIN_INTERRUPCION)
            log_debug(logger_cpu, "Hubo interrupcion pero para otro pid. PID en ejeucion: %i - PID de interrupcion: %i", pcb->pid, pid_interrupcion);
        pthread_mutex_unlock(&interrupcion_mutex);
    }

    return seguir;
}


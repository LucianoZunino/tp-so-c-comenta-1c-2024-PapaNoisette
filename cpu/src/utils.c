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

t_instruccion * fetch(){
    //void *buffer = recibir_buffer(cliente_socket);
    //send(cliente_socket, buffer, 5, NULL);
    t_instruccion * instruccion;
    return instruccion;
    
}

t_instruction_code decode(char *instruccion)
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
        else if (strcmp(instruccion, "JNZ") == 0)
        {
            return JNZ;
        }
        else if (strcmp(instruccion, "RESIZE") == 0)
        {
            return RESIZE;
        }
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
            log_error(logger_cpu, "La intrucción no es válidaa");
                                    printf("holaaa %s"),instruccion;

            return -1; // supuestamente las instrucciones nunca vienen incorrectas
        }
    }

void ejecutar_set(t_instruccion * instruccion)
    {
        if (strcmp(instruccion->arg1, "AX") == 0)
        {
        AX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "BX") == 0)
        {
        BX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "CX") == 0)
        {
        CX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "DX") == 0)
        {
        DX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "EAX") == 0)
        {
        EAX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "EBX") == 0)
        {
        EBX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "ECX") == 0)
        {
        ECX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "EDX") == 0)
        {
        EDX=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "SI") == 0)
        {
        SI=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "DI") == 0)
        {
        DI=atoi(instruccion->arg2);        
        }
        else if (strcmp(instruccion->arg1, "PC") == 0)
        {
        PROGRAM_COUNTER=atoi(instruccion->arg2);        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SET es incorrecto");
        }
    }

void ejecutar_sum(t_instruccion * instruccion)
    {
         int sum =atoi(instruccion->arg1)+atoi(instruccion->arg2);
        printf ("sum: %d\n",sum);
        if (strcmp(instruccion->arg1, "AX") == 0)
        {
            AX=sum;
        }
        else if (strcmp(instruccion->arg1, "BX") == 0)
        {
            BX=sum;
        }
        else if (strcmp(instruccion->arg1, "CX") == 0)
        {
            CX=sum;
        }
        else if (strcmp(instruccion->arg1, "DX") == 0)
        {
            DX=sum;
        }
        else if (strcmp(instruccion->arg1, "EAX") == 0)
        {
            EAX=sum;
        }
        else if (strcmp(instruccion->arg1, "EBX") == 0)
        {
            EBX=sum;
        }
        else if (strcmp(instruccion->arg1, "ECX") == 0)
        {
            ECX=sum;
        }
        else if (strcmp(instruccion->arg1, "EDX") == 0)
        {
            EDX=sum;
        }
        else if (strcmp(instruccion->arg1, "SI") == 0)
        {
            SI=sum;
        }
        else if (strcmp(instruccion->arg1, "DI") == 0)
        {
            DI=sum;
        }
        else if (strcmp(instruccion->arg1, "PROGRAM_COUNTER") == 0)
        {
        PROGRAM_COUNTER=sum;        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SUM es incorrecto");
        }
    }
void ejecutar_sub(t_instruccion * instruccion)
    {
        int sub =atoi(instruccion->arg1)-atoi(instruccion->arg2);
        printf ("sub: %d\n",sub);
        if (strcmp(instruccion->arg1, "AX") == 0)
        {
            AX=sub;
        }
        else if (strcmp(instruccion->arg1, "BX") == 0)
        {
            BX=sub;
        }
        else if (strcmp(instruccion->arg1, "CX") == 0)
        {
            CX=sub;
        }
        else if (strcmp(instruccion->arg1, "DX") == 0)
        {
            DX=sub;
        }
        else if (strcmp(instruccion->arg1, "EAX") == 0)
        {
            EAX=sub;
        }
        else if (strcmp(instruccion->arg1, "EBX") == 0)
        {
            EBX=sub;
        }
        else if (strcmp(instruccion->arg1, "ECX") == 0)
        {
            ECX=sub;
        }
        else if (strcmp(instruccion->arg1, "EDX") == 0)
        {
            EDX=sub;
        }
        else if (strcmp(instruccion->arg1, "SI") == 0)
        {
            SI=sub;
        }
        else if (strcmp(instruccion->arg1, "DI") == 0)
        {
            DI=sub;
        }
        else if (strcmp(instruccion->arg1, "PROGRAM_COUNTER") == 0)
        {
        PROGRAM_COUNTER=sub;        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SUB es incorrecto");
        }
    }

void ejecutar_jnz(t_instruccion* instruccion){

    int new_program_counter=atoi(instruccion->arg2);
        if (strcmp(instruccion->arg1, "AX") == 0 && AX ==0)
        {
            PROGRAM_COUNTER=new_program_counter; 
        }
        else if (strcmp(instruccion->arg1, "BX") == 0 && BX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "CX") == 0 && CX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;        
        }
        else if (strcmp(instruccion->arg1, "DX") == 0 && DX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "EAX") == 0 && EAX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "EBX") == 0 && EBX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "ECX") == 0 && ECX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "EDX") == 0 && EDX ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "SI") == 0 && SI ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else if (strcmp(instruccion->arg1, "DI") == 0 && DI ==0)
        {
            PROGRAM_COUNTER=new_program_counter;
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción JNZ es incorrecto");
        }
}

void  ejecutar_io_gen_sleep(t_instruccion * instruccion){
//IO_GEN_SLEEP (Interfaz, Unidades de trabajo): Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.
}

//Es capaz de resolver las operaciones: SET, SUM, SUB, JNZ e IO_GEN_SLEEP.

void ejecutar_instruccion(t_instruccion *instruccion ){// ver que debe retornar esta funcion...

            PROGRAM_COUNTER += 1;
            t_instruction_code instruction_code = decode(instruccion->instruccion);

            loggear_ejecucion(instruccion);

            switch (instruction_code)
            {
            case SET://--------<>
                ejecutar_set(instruccion);
                break;
            case MOV_IN:
                break;
            case MOV_OUT:
                break;
            case SUM://---------<>
                ejecutar_sum(instruccion);
                break;
            case SUB://---------<>
                ejecutar_sub(instruccion);
                break;
            case JNZ://---------<>
                ejecutar_jnz(instruccion);
                break;
            case RESIZE:
                break;
            case COPY_STRING:
                break;
            case WAIT:
                break;
            case SIGNAL:
                break;
            case IO_GEN_SLEEP://---------<>
                ejecutar_io_gen_sleep(instruccion);
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

        }
    
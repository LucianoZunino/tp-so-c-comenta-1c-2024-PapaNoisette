
#include "instrucciones.h"


void ejecutar_set(char *registro,char* valor,t_proceso_cpu *proceso)
    {
        if (strcmp(registro, "AX") == 0)
        {
       proceso->registros_cpu->AX=atoi(valor);        
        }
        else if (strcmp(registro, "BX") == 0)
        {
       proceso->registros_cpu->BX=atoi(valor);        
        }
        else if (strcmp(registro, "CX") == 0)
        {
       proceso->registros_cpu->CX=atoi(valor);        
        }
        else if (strcmp(registro, "DX") == 0)
        {
       proceso->registros_cpu->DX=atoi(valor);        
        }
        else if (strcmp(registro, "EAX") == 0)
        {
       proceso->registros_cpu->EAX=atoi(valor);        
        }
        else if (strcmp(registro, "EBX") == 0)
        {
       proceso->registros_cpu->EBX=atoi(valor);        
        }
        else if (strcmp(registro, "ECX") == 0)
        {
      proceso->registros_cpu->ECX=atoi(valor);        
        }
        else if (strcmp(registro, "EDX") == 0)
        {
       proceso->registros_cpu->EDX=atoi(valor);        
        }
        else if (strcmp(registro, "SI") == 0)
        {
       proceso->registros_cpu->SI=atoi(valor);        
        }
        else if (strcmp(registro, "DI") == 0)
        {
       proceso->registros_cpu->DI=atoi(valor);        
        }
        else if (strcmp(registro, "PC") == 0)
        {
       proceso->registros_cpu->PROGRAM_COUNTER=atoi(valor);        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SET es incorrecto");
        }
    }

void ejecutar_sum(char*registro_destino,char *registro_origen,t_proceso_cpu *proceso)
    {
         int sum =atoi(registro_destino)+atoi(registro_origen);
        printf ("sum: %d\n",sum);
        if (strcmp(registro_destino, "AX") == 0)
        {
           proceso->registros_cpu->AX=sum;
        }
        else if (strcmp(registro_destino, "BX") == 0)
        {
           proceso->registros_cpu->BX=sum;
        }
        else if (strcmp(registro_destino, "CX") == 0)
        {
           proceso->registros_cpu->CX=sum;
        }
        else if (strcmp(registro_destino, "DX") == 0)
        {
           proceso->registros_cpu->DX=sum;
        }
        else if (strcmp(registro_destino, "EAX") == 0)
        {
           proceso->registros_cpu->EAX=sum;
        }
        else if (strcmp(registro_destino, "EBX") == 0)
        {
           proceso->registros_cpu->EBX=sum;
        }
        else if (strcmp(registro_destino, "ECX") == 0)
        {
           proceso->registros_cpu->ECX=sum;
        }
        else if (strcmp(registro_destino, "EDX") == 0)
        {
           proceso->registros_cpu->EDX=sum;
        }
        else if (strcmp(registro_destino, "SI") == 0)
        {
           proceso->registros_cpu->SI=sum;
        }
        else if (strcmp(registro_destino, "DI") == 0)
        {
           proceso->registros_cpu->DI=sum;
        }
        else if (strcmp(registro_destino, "PROGRAM_COUNTER") == 0)
        {
       proceso->registros_cpu->PROGRAM_COUNTER=sum;        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SUM es incorrecto");
        }
    }
void ejecutar_sub(char*registro_destino,char *registro_origen,t_proceso_cpu *proceso)
    {
         int sum =atoi(registro_destino)-atoi(registro_origen);
        printf ("sub: %d\n",sum);
        if (strcmp(registro_destino, "AX") == 0)
        {
           proceso->registros_cpu->AX=sum;
        }
        else if (strcmp(registro_destino, "BX") == 0)
        {
           proceso->registros_cpu->BX=sum;
        }
        else if (strcmp(registro_destino, "CX") == 0)
        {
           proceso->registros_cpu->CX=sum;
        }
        else if (strcmp(registro_destino, "DX") == 0)
        {
           proceso->registros_cpu->DX=sum;
        }
        else if (strcmp(registro_destino, "EAX") == 0)
        {
           proceso->registros_cpu->EAX=sum;
        }
        else if (strcmp(registro_destino, "EBX") == 0)
        {
           proceso->registros_cpu->EBX=sum;
        }
        else if (strcmp(registro_destino, "ECX") == 0)
        {
           proceso->registros_cpu->ECX=sum;
        }
        else if (strcmp(registro_destino, "EDX") == 0)
        {
           proceso->registros_cpu->EDX=sum;
        }
        else if (strcmp(registro_destino, "SI") == 0)
        {
           proceso->registros_cpu->SI=sum;
        }
        else if (strcmp(registro_destino, "DI") == 0)
        {
           proceso->registros_cpu->DI=sum;
        }
        else if (strcmp(registro_destino, "PROGRAM_COUNTER") == 0)
        {
       proceso->registros_cpu->PROGRAM_COUNTER=sum;        
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción SUM es incorrecto");
        }
    }
void ejecutar_jnz(char*registro,char *numero_instruccion,t_proceso_cpu *proceso){

        if (strcmp(registro, "AX") == 0 && proceso->registros_cpu->AX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion); 
        }
        else if (strcmp(registro, "BX") == 0 && proceso->registros_cpu->BX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "CX") == 0 && proceso->registros_cpu->CX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);        
        }
        else if (strcmp(registro, "DX") == 0 && proceso->registros_cpu->DX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "EAX") == 0 && proceso->registros_cpu->EAX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "EBX") == 0 && proceso->registros_cpu->EBX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "ECX") == 0 && proceso->registros_cpu->ECX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "EDX") == 0 && proceso->registros_cpu->EDX !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "SI") == 0 && proceso->registros_cpu->SI !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else if (strcmp(registro, "DI") == 0 && proceso->registros_cpu->DI !=0)
        {
            proceso->registros_cpu->PROGRAM_COUNTER=atoi(numero_instruccion);
        }
        else
        {
            log_error(logger_cpu, "El argumento de la instrucción JNZ es incorrecto");
        }
}

void  ejecutar_io_gen_sleep(char* interfaz,char * unidades_de_trabajo){
//IO_GEN_SLEEP (Interfaz, Unidades de trabajo):
 //Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.instruccion

    log_info(logger_cpu, " ENVIANDO solicitud de sleep A KERNEL");
   
    t_buffer* buffer_a_enviar = crear_buffer();

     cargar_string_al_buffer(buffer_a_enviar, interfaz);//interfaz io  que debe hacer el sleep
     cargar_string_al_buffer(buffer_a_enviar, unidades_de_trabajo); //tiempo en ms de sleep

     t_paquete* paquete = crear_paquete(IO_GEN_SLEEP, buffer_a_enviar);
     enviar_paquete(paquete, fd_kernel_dispatch);
     eliminar_paquete(paquete);
}

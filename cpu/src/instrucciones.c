
#include "instrucciones.h"


void ejecutar_set(char *registro,char* valor,t_pcb *proceso)
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

void ejecutar_sum(char*registro_destino,char *registro_origen,t_pcb *proceso)
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
void ejecutar_sub(char*registro_destino,char *registro_origen,t_pcb *proceso)
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
void ejecutar_jnz(char*registro,char *numero_instruccion,t_pcb *proceso){

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

void  ejecutar_io_gen_sleep(char* interfaz,char * unidades_de_trabajo,t_pcb *proceso){
//IO_GEN_SLEEP (Interfaz, Unidades de trabajo):
 //Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.instruccion

    log_info(logger_cpu, " ENVIANDO solicitud de sleep A KERNEL");
   
    t_buffer* buffer_a_enviar = crear_buffer();

     cargar_string_al_buffer(buffer_a_enviar, interfaz);//interfaz io  que debe hacer el sleep
     cargar_int_al_buffer(buffer_a_enviar, atoi(unidades_de_trabajo)); //tiempo en ms de sleep

     t_paquete* paquete = crear_paquete(IO_GEN_SLEEP_FS, buffer_a_enviar);
     enviar_paquete(paquete, fd_kernel_dispatch);
     eliminar_paquete(paquete);
}


int ejecutar_resize(char * tamanio,t_pcb *proceso){
   /*Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro.
    En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto 
    de ejecución al Kernel informando de esta situación.*/

    log_info(logger_cpu, " ENVIANDO RESIZE A MEMORIA");
   
    t_buffer* buffer_a_enviar = crear_buffer();

     cargar_int_al_buffer(buffer_a_enviar, proceso->pid);//Proceso al que se le reajustara su tamaño en memoria
     cargar_int_al_buffer(buffer_a_enviar, atoi(tamanio)); //nuevo tamaño del proceso

     t_paquete* paquete = crear_paquete(MEMORIA_RESIZE, buffer_a_enviar);
     enviar_paquete(paquete, fd_memoria);
     eliminar_paquete(paquete);

     int respuesta_resize = recibir_operacion(fd_memoria);//respuesta_resize deveria ser del tipo op_code?
     if (respuesta_resize ==RESIZE_OK)
     {
       log_info(logger_cpu, "Se realizo correctamente el resize");
       return 0;
     }
   if (respuesta_resize ==OUT_OF_MEMORY){
      log_info(logger_cpu, "Fallo el resize por OUT OF MEMORY");
      return 1;
   }


}

void ejecutar_copy_string(char* tamanio,t_pcb *proceso){
/*
Toma del string apuntado por el registro SI y copia la cantidad de bytes 
indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI. 
*/
    int dir_fisica=traducir_direccion_logica(proceso->registros_cpu->SI);
    log_info(logger_cpu, " ENVIANDO EJECUTANDO COPY_STRING");
        log_info(logger_cpu, "Le solicito a memoria leer lo que se encuentra en el puntero SI");
      t_buffer* buffer_a_enviar = crear_buffer();
      //Le mando el pid por que quizas memoria deberia validar que no invada un espacio incorrecto 
      cargar_int_al_buffer(buffer_a_enviar, proceso->pid);
      cargar_int_al_buffer(buffer_a_enviar, dir_fisica);//le deberia mandar tambien el tamanio a leer??

      t_paquete* paquete = crear_paquete(MEMORIA_LEER, buffer_a_enviar);
      enviar_paquete(paquete, fd_memoria);
      eliminar_paquete(paquete);

    log_info(logger_cpu, " ENVIANDO RESIZE A MEMORIA");


}

//en estas dos intrucciones no se si es necesario mandar el mensaje o directamente 
//enviar el contexto de ejecucion con el motivo wait/signal

void ejecutar_wait(char* recurso,t_pcb *proceso){
   log_info(logger_cpu, " ENVIANDO WAIT A KERNEL");
   
    t_buffer* buffer_a_enviar = crear_buffer();

     cargar_int_al_buffer(buffer_a_enviar, proceso->pid);//Proceso al que se le reajustara su tamaño en memoria
     t_paquete* paquete = crear_paquete(KERNEL_WAIT, buffer_a_enviar);
     enviar_paquete(paquete, fd_kernel_dispatch);
     eliminar_paquete(paquete);
}
void ejecutar_signal(char* recurso,t_pcb *proceso){
 log_info(logger_cpu, " ENVIANDO SIGNAL A KERNEL");
   
    t_buffer* buffer_a_enviar = crear_buffer();

     cargar_int_al_buffer(buffer_a_enviar, proceso->pid);//Proceso al que se le reajustara su tamaño en memoria
     t_paquete* paquete = crear_paquete(KERNEL_SIGNAL, buffer_a_enviar);
     enviar_paquete(paquete, fd_kernel_dispatch);
     eliminar_paquete(paquete);

}
void ejecutar_io_stdin_read(char* interfaz,char * direccion, char * tamanio,t_pcb *proceso){
/*
Esta instrucción solicita al Kernel que mediante la interfaz ingresada se lea desde el
 STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño
  y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.

*/

}
void ejecutar_io_stdout_write(char* interfaz,char * direccion, char * tamanio,t_pcb *proceso){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde la
    posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección, 
    un tamaño indicado por el Registro Tamaño y se imprima por pantalla.

   */
}

void ejecutar_mov_in(char* datos,char * direccion,t_pcb *proceso){

/*
 Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro 
 Dirección y lo almacena en el Registro Datos.

*/
}
void ejecutar_mov_out(char* direccion,char * datos,t_pcb *proceso){

/*
 Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida a 
 partir de la Dirección Lógica almacenada en el Registro Dirección.

*/
}


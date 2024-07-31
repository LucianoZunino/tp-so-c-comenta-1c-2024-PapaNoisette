#include "iniciar_cpu.h"

t_config *config_cpu;
t_list* tlb;

void iniciar_cpu()
{
   iniciar_logger_cpu();
   iniciar_config_cpu();
   imprimir_config_cpu();
   tlb = list_create();
	EXEC=malloc(sizeof(t_pcb));
	EXEC->registros_cpu=malloc(sizeof(t_registros_cpu));
	EXEC->estado=malloc(sizeof(estado_pcb));
	//iniciar_pcb(); //ESTA BIEN ESTO?
}

//void iniciar_pcb(){
//   EXEC->estado=0;
//	EXEC->pid=0;
//	EXEC->program_counter=0;
//	EXEC->quantum=0;
//	EXEC->registros_cpu->AX=0;
//   EXEC->registros_cpu->BX=0;
//	EXEC->registros_cpu->CX=0;
//   EXEC->registros_cpu->DX=0;
//   EXEC->registros_cpu->EAX=0;
//	EXEC->registros_cpu->EBX=0;
//	EXEC->registros_cpu->ECX=0;
//   EXEC->registros_cpu->EDX=0;
//	EXEC->registros_cpu->SI=0;
//	EXEC->registros_cpu->DI=0;

	

//}
void iniciar_logger_cpu()
{
   logger_cpu = iniciar_logger("cpu.log", "cpu");
}

void iniciar_config_cpu(){
   config_cpu = iniciar_config("cpu.config");

   ip_memoria = config_get_string_value(config_cpu, "IP_MEMORIA");
   puerto_memoria = config_get_string_value(config_cpu, "PUERTO_MEMORIA");
   puerto_escucha_dispatch = config_get_string_value(config_cpu, "PUERTO_ESCUCHA_DISPATCH");
   puerto_escucha_interrupt = config_get_string_value(config_cpu, "PUERTO_ESCUCHA_INTERRUPT");
   cantidad_entradas_tlb = config_get_int_value(config_cpu, "CANTIDAD_ENTRADAS_TLB");
   algoritmo_tlb = config_get_string_value(config_cpu, "ALGORITMO_TLB");
}
/*
void loggear_ejecucion(t_instruccion *instruccion) // log obligatorio
{
   log_info(logger_cpu, "Ejecutando: %s - arg1: %s, arg2: %s, arg3: %s",
            instruccion->instruccion,
            instruccion->arg1,
            instruccion->arg2,
            instruccion->arg3);
}*/

void imprimir_config_cpu(){
   printf("\n============================================================\n");
   printf("IP de Memoria: %s\n", ip_memoria);
   printf("Puerto de Memoria: %s\n", puerto_memoria);
   printf("Puerto de escucha de Dispatch: %s\n", puerto_escucha_dispatch);
   printf("Puerto de escucha de Interrupt: %s\n", puerto_escucha_interrupt);
   printf("Cantidad de entradas de la TLB: %d\n", cantidad_entradas_tlb);
   printf("Algortimo de la TLB: %s\n", algoritmo_tlb);
   printf("============================================================\n\n");
}

void finalizar_cpu(){
   config_destroy(config_cpu);
   log_destroy(logger_cpu);
}

int get_tamanio_registro(char *registro){
   if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 ||
       strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0)
   {
      return 1;
   }
   else{
      return 4;
   }
}

uint32_t get_registro(char *registro)
{

   if (strcmp(registro, "AX") == 0)
   {
      return EXEC->registros_cpu->AX;
   }
   else if (strcmp(registro, "BX") == 0)
   {
      return EXEC->registros_cpu->BX;
   }
   else if (strcmp(registro, "CX") == 0)
   {
      return EXEC->registros_cpu->CX;
   }
   else if (strcmp(registro, "DX") == 0)
   {
      return EXEC->registros_cpu->DX;
   }
   else if (strcmp(registro, "EAX") == 0)
   {
      return EXEC->registros_cpu->EAX;
   }
   else if (strcmp(registro, "EBX") == 0)
   {
      return EXEC->registros_cpu->EBX;
   }
   else if (strcmp(registro, "ECX") == 0)
   {
      return EXEC->registros_cpu->ECX;
   }
   else if (strcmp(registro, "EDX") == 0)
   {
      return EXEC->registros_cpu->EDX;
   }
   else if (strcmp(registro, "SI") == 0)
   {
      return EXEC->registros_cpu->SI;
   }
   else if (strcmp(registro, "DI") == 0)
   {
      return EXEC->registros_cpu->DI;
   }
   else if (strcmp(registro, "PROGRAM_COUNTER") == 0)
   {
      return EXEC->program_counter;
   }
   else
   {
      printf("Error en get_registro\n");
   }
}

int consultar_tamanio_pagina_memoria()
{ // deberia estar luego del hanshake con memoria,o mejor en iniciar_cpu o qcyo
   log_info(logger_cpu, "consultar_tamanio_pagina_memoria" );

   t_buffer *buffer_a_enviar = crear_buffer();
   t_paquete *paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
   t_buffer *buffer = crear_buffer();

   int nuevo_tam_pagina = -1; // inicializo asi para saber que no llego la respuesta

   while (nuevo_tam_pagina == -1) 
   {

      // si memoria no me contesta estoy al horno
      int cod_op = recibir_operacion(fd_memoria);

      if(cod_op == CPU_CONSULTA_TAM_PAGINA)
      {

         buffer = recibir_buffer_completo(fd_memoria);
         nuevo_tam_pagina = extraer_int_del_buffer(buffer);
      }
   }
      log_info(logger_cpu, "SE RECIBIO DE MEMORIA EL TAMAÑO DE PAGINA= %d", nuevo_tam_pagina);
      destruir_buffer(buffer);
      printf("destruir_buffer\n");

   return nuevo_tam_pagina;
}


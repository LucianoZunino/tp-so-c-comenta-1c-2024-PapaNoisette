
#include "mmu.h"
void mostrar_entrada(t_entrada_tlb*entrada ){
printf("pid :%d  pagina: %d marco: %d\n",entrada->pid,entrada->pagina,entrada->marco);

}
void mostrar_tlb(){
printf("Mostrar TLB: \n");

list_iterate(tlb,mostrar_entrada);

}

int traducir_direccion_logica(int a){
   return a;
}

int calcular_numero_de_pagina(int dir_logica)
{
   return floor(dir_logica / tamanio_pagina);
}

int calcular_desplazamiento(int dir_logica, int numero_pagina)
{
   return dir_logica - numero_pagina * tamanio_pagina;
}

int consultar_tamanio_pagina_memoria()
{ // deberia estar luego del hanshake con memoria,o mejor en iniciar_cpu o qcyo
      printf("flag 1 consultar tamanio: \n");

   t_buffer *buffer_a_enviar = crear_buffer();
   t_paquete *paquete = crear_paquete(CPU_CONSULTA_TAM_PAGINA, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
t_buffer * buffer=crear_buffer();
      printf("flag 2 consultar tamanio: \n");

   int nuevo_tam_pagina = -1; // inicializo asi para saber que no llego la respuesta
               printf("flag 3 consultar tamanio: \n");

   while (nuevo_tam_pagina == -1)
   {
            printf("flag 4 consultar tamanio: \n");

      // si memoria no me contesta estoy al horno
      int cod_op = recibir_operacion(fd_memoria);
            printf("flag 5 consultar tamanio: \n");

      if (cod_op == CPU_CONSULTA_FRAME)
         {
                        printf("flag 6 consultar tamanio: \n");

            buffer = recibir_buffer_completo(fd_memoria);
            nuevo_tam_pagina=extraer_int_del_buffer(buffer);
         }
      
   }

return 0;
}

void agregar_entrada_tlb(t_entrada_tlb *entrada)
{

   if (list_size(tlb) < cantidad_entradas_tlb) // si todavia no se lleno la TLB
   {
      list_add(tlb, entrada);// en LRU el recently va al final
   }
   else
   {
      // en este caso debo remplazar la entrada
      printf("Se realizara un remplazo en la tlb: \n");

      list_remove(tlb, 0); // sea lru o fifo  en la posicion 0 esta el mas viejo
      list_add(tlb, entrada);
   }
}


//-1 :tlb deshabilitada       obtener_marco_en_memoria(pid, pagina);
// 1 :tlb miss       obtener_marco_en_memoria(pid, pagina);
// x>0 tlb hit
int buscar_marco_tlb(int pid, int dir_logica)
{ // la funcion principal de mmu
   int marco=-1;
   int pagina = calcular_numero_de_pagina(dir_logica);


   // chequeo que la TLB este habilitada
   if (cantidad_entradas_tlb == 0)
   {
      log_info(logger_cpu, "La tlb esta deshabilitada\n");
      return -1;
   }


   // busco en la tlb
   int _closure_find_entrada(t_entrada_tlb * entrada_find)
   {
      if (entrada_find->pid == pid && entrada_find->pagina == pagina)
         return 1;
   }
   t_entrada_tlb *entrada = list_find(tlb, _closure_find_entrada);

   // TLB MISS
   if (entrada == NULL) 
   {
      //LOG OBLIGATORIO
      log_info(logger_cpu, "PID: %d -TLB MISS - Pagina: %d\n", pid, pagina);
      return 1;
   }
   else
   //TLB HIT
   {
      //LOG OBLIGATORIO
      log_info(logger_cpu, "PID: %d -TLB HIT - Pagina: %d", pid, pagina);
printf("algoritmo :%s\n",algoritmo_tlb);
      //REAJUSTAMOS LA TLB SI ES LRU     
       if (strcmp(algoritmo_tlb,"LRU")==0){

         printf("reajuste tlb por LRU \n");

         //lo mando al final de la lista por que es ahora el mas reciente
         list_remove_element(tlb,entrada);//test ok
         list_add(tlb,entrada);
      }
   }
         return 0;

}


int obtener_marco_en_memoria(int pid, int pagina)//obtengo el marco en memoria
{
printf("Se consultara a memoria el marco correspondiente al PID: %d  y la PAGINA: %d\n",pid, pagina);

   // 1- consultar a la memoria para obtener el frame correspondiente a la página del proceso buscada.

   t_buffer *buffer_a_enviar = crear_buffer();
   t_paquete *paquete = crear_paquete(CPU_CONSULTA_FRAME, buffer_a_enviar);
   cargar_int_al_buffer(paquete->buffer, pid);
   cargar_int_al_buffer(paquete->buffer, pagina);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
t_buffer * buffer=crear_buffer();

   int marco = -1; // inicializo asi para saber que no llego la respuesta
   while (marco == -1)
   {
      // si memoria no me contesta estoy al horno
      int cod_op = recibir_operacion(fd_memoria);

      if (cod_op == CPU_CONSULTA_FRAME)
         {
            buffer = recibir_buffer_completo(fd_memoria);
            marco=extraer_int_del_buffer(buffer);
         }
      
   }

   // 2-Agregarmos la nueva entrada a tlb (si esta habilitada)
   if (cantidad_entradas_tlb > 0)
      {
         t_entrada_tlb *nueva_entrada = malloc(sizeof(t_entrada_tlb));
         nueva_entrada->pid = pid;
         nueva_entrada->pagina = pagina;
         nueva_entrada->marco = marco;

         agregar_entrada_tlb(nueva_entrada);
      }
   //LOG OBLIGATORIO
   log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d\n", pid, pagina, marco);

}
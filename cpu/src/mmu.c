#include "mmu.h"

void mostrar_entrada(t_entrada_tlb*entrada ){
   printf("pid :%d  pagina: %d marco: %d\n", entrada->pid, entrada->pagina, entrada->marco);
}

void mostrar_tlb(){
   printf("Mostrar TLB: \n");
   list_iterate(tlb,mostrar_entrada);
}

/*---------------------------------------- TLB ----------------------------------------*/

/*HACER TRADUCCION LOGICA A FISICA*/

double calcular_numero_de_pagina(int dir_logica){
   return floor(dir_logica / tamanio_pagina);
}

int calcular_desplazamiento(int dir_logica, int numero_pagina){
   return dir_logica - numero_pagina * tamanio_pagina;
}


uint32_t traducir_direccion_logica(int dir_logica){
   
   int pagina = (int)calcular_numero_de_pagina(dir_logica);
   int offset = calcular_desplazamiento(dir_logica,pagina);
   int pid = EXEC->pid;
   int marco = -1;
   
   if(marco_en_tlb(pid, pagina, &marco)){
      log_info(logger_cpu, "TLB Hit: \"PID: %d - TLB HIT - Página: %d\n\"", pid, pagina);
      printf("Marco en TLB HIT: %i", marco);
      return calculo_dir_fisica(marco, offset);
   }

   log_info(logger_cpu,"TLB Miss: \"PID: %d - TLB Miss - Página: %d\n\"", pid, pagina);
   
   t_buffer* buffer = crear_buffer();
   cargar_int_al_buffer(buffer, pid);
   cargar_int_al_buffer(buffer, pagina);
   t_paquete* paquete = crear_paquete(CPU_CONSULTA_FRAME, buffer);
   enviar_paquete(paquete, fd_memoria);

   while(marco == -1){
      int cod_op = recibir_operacion(fd_memoria);

      if(cod_op == CPU_CONSULTA_FRAME){
         buffer = recibir_buffer_completo(fd_memoria);
         marco = extraer_int_del_buffer(buffer);

         log_info(logger_cpu, "Obtener Marco: \"PID: %d - OBTENER MARCO - Página: %d - Marco: %d\"\n", pid, pagina, marco);

      }
   }
   
   eliminar_paquete(paquete);

   if(cantidad_entradas_tlb != 0){
      agregar_entrada_tlb(pid, pagina, marco);
   }

   return calculo_dir_fisica(marco, offset);
}

uint32_t calculo_dir_fisica(int marco, int offset){
   uint32_t direccion_fisica = marco * tamanio_pagina + offset;
   return direccion_fisica;
}

void agregar_entrada_tlb(int pid,int pagina,int marco){
   t_entrada_tlb* nueva_entrada = malloc(sizeof(t_entrada_tlb));
   nueva_entrada->pid = pid;
   nueva_entrada->pagina = pagina;
   nueva_entrada->marco = marco;
   int size_actual_tlb = list_size(tlb);
   
   if((size_actual_tlb == cantidad_entradas_tlb)){
      t_entrada_tlb* vieja_entrada = list_remove(tlb, 0);
      free(vieja_entrada);
   }

   list_add(tlb, nueva_entrada);
}


bool marco_en_tlb(int pid, int pagina, int* marco){
   if(cantidad_entradas_tlb == 0){
      return false;
   }

   int i = 0;
   
   while((i < list_size(tlb)) && (i < cantidad_entradas_tlb)){
      //t_entrada_tlb* entrada_actual = malloc(sizeof(t_entrada_tlb));
      t_entrada_tlb* entrada_actual = list_get(tlb, i);
      
      if((entrada_actual->pid == pid) && (entrada_actual->pagina == pagina)){
         *marco = entrada_actual->marco;
         actualizar_tlb_lru(i); 
         return true;
      }

      i++; 
   }

   return false;
}

void actualizar_tlb_lru(int index){
   if(string_equals_ignore_case(algoritmo_tlb, "LRU")){
      t_entrada_tlb* entrada_a_actualizar = list_remove(tlb, index);
      list_add(tlb, entrada_a_actualizar);
   }
}

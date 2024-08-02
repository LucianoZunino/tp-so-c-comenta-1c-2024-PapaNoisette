#include "instrucciones.h"

void ejecutar_set(char *registro, int valor){
   //log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %a - Ejecutando: %d\”ejecutando set,  valor: %d, registro destino %s\n\"", valor, registro);

   if (strcmp(registro, "AX") == 0)
   {
      EXEC->registros_cpu->AX = (uint8_t) valor;
   }
   else if (strcmp(registro, "BX") == 0)
   {
      EXEC->registros_cpu->BX = (uint8_t) valor;
   }
   else if (strcmp(registro, "CX") == 0)
   {
      EXEC->registros_cpu->CX = (uint8_t) valor;
   }
   else if (strcmp(registro, "DX") == 0)
   {
      EXEC->registros_cpu->DX = (uint8_t) valor;
   }
   else if (strcmp(registro, "EAX") == 0)
   {
      EXEC->registros_cpu->EAX = (uint32_t) valor;
   }
   else if (strcmp(registro, "EBX") == 0)
   {
      EXEC->registros_cpu->EBX = (uint32_t) valor;
   }
   else if (strcmp(registro, "ECX") == 0)
   {
      EXEC->registros_cpu->ECX = (uint32_t) valor;
   }
   else if (strcmp(registro, "EDX") == 0)
   {
      EXEC->registros_cpu->EDX = (uint32_t) valor;
   }
   else if (strcmp(registro, "SI") == 0)
   {
      EXEC->registros_cpu->SI = (uint32_t) valor;
   }
   else if (strcmp(registro, "DI") == 0)
   {
      EXEC->registros_cpu->DI = (uint32_t) valor;
   }
   else if (strcmp(registro, "PC") == 0)
   {
      EXEC->program_counter = (uint32_t) valor;
   }
   else
   {
      log_error(logger_cpu, "El argumento de la instrucción SET es incorrecto");
   }
}

void ejecutar_sum(char *registro_destino, char *registro_origen){
   int sum = get_registro(registro_destino) + get_registro(registro_origen);
   ejecutar_set(registro_destino, sum);
}

void ejecutar_sub(char *registro_destino, char *registro_origen){
   int sub = get_registro(registro_destino) - get_registro(registro_origen);
   ejecutar_set(registro_destino, sub);
}

void ejecutar_jnz(char *registro, char *numero_instruccion){
   if(get_registro(registro) != 0){
      ejecutar_set("PC", atoi(numero_instruccion));
   }
}

/*
   Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro.
   En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto
   de ejecución al Kernel informando de esta situación.
*/
int ejecutar_resize(char *tamanio){
   t_buffer *buffer_a_enviar = crear_buffer();

   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);  // Proceso al que se le reajustara su tamaño en memoria
   cargar_int_al_buffer(buffer_a_enviar, atoi(tamanio)); // nuevo tamaño del proceso

   t_paquete *paquete = crear_paquete(MEMORIA_RESIZE, buffer_a_enviar);
   log_info(logger_cpu, " ENVIANDO RESIZE A MEMORIA pid %d size: %d\n",EXEC->pid,atoi(tamanio));

   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);

   //--tiempo de retardo establecido 
   t_buffer *buffer = crear_buffer();

   sleep(2);
   int respuesta_resize = recibir_operacion(fd_memoria); // respuesta_resize deveria ser del tipo op_code?
   if (respuesta_resize == RESIZE_OK)
   {

      log_info(logger_cpu, "Se realizo correctamente el resize");
      return 0;
   }
   if (respuesta_resize == OUT_OF_MEMORY)
   {
      log_info(logger_cpu, "Fallo el resize por OUT OF MEMORY");
      // devuelvo el contexto a kernel
      t_buffer *buffer_out_of_memory = crear_buffer();
      t_paquete *paquete_kernel = crear_paquete(OUT_OF_MEMORY, buffer_out_of_memory);
      agregar_pcb(paquete_kernel, EXEC);
      enviar_paquete(paquete_kernel, fd_kernel_dispatch);
      eliminar_paquete(paquete_kernel);

      return 1;
   }
   sleep(2);

}

void ejecutar_copy_string(char *tamanio){
   int dir_fisica_si = (int) traducir_direccion_logica(EXEC->registros_cpu->SI);
   int dir_fisica_di = (int) traducir_direccion_logica(EXEC->registros_cpu->DI);

   t_buffer *buffer_a_enviar_leer = crear_buffer();
   cargar_int_al_buffer(buffer_a_enviar_leer, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar_leer, atoi(tamanio));
   cargar_int_al_buffer(buffer_a_enviar_leer, dir_fisica_si);
   cargar_int_al_buffer(buffer_a_enviar_leer, dir_fisica_di);

   t_paquete *paquete_leer = crear_paquete(MEMORIA_COPY_STRING, buffer_a_enviar_leer);
   enviar_paquete(paquete_leer, fd_memoria);
   
   eliminar_paquete(paquete_leer);
}

void ejecutar_wait(char *recurso){
   log_info(logger_cpu, "ENVIANDO WAIT A KERNEL");

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(KERNEL_WAIT, buffer);
  //devuelvo contexto por solicitar llamada a kernel
   printf("---- PID a agregar en ejecutar_wait %i ----\n", EXEC->pid);
   agregar_pcb(paquete, EXEC);
   cargar_string_al_buffer(paquete->buffer, recurso);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_signal(char *recurso){
   log_info(logger_cpu, "ENVIANDO SIGNAL A KERNEL");

  //devuelvo contexto por solicitar llamada a kernel

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(KERNEL_SIGNAL, buffer);
   agregar_pcb(paquete, EXEC);
   cargar_string_al_buffer(paquete->buffer, recurso);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

/*
   Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro
   Dirección y lo almacena en el Registro Datos.
*/
void ejecutar_mov_in(char *registro_datos, char *registro_direccion){
   log_info(logger_cpu, "ENVIANDO MOV IN A MEMORIA");
   int dir_logica = -1;
   dir_logica = get_registro(registro_direccion); // obtengo la dir logica
   int tam_registro = get_tamanio_registro(registro_datos); //calculo el tamaño a leer en memoria 
   uint32_t dir_fisica = traducir_direccion_logica(dir_logica);

   t_buffer *buffer_a_enviar = crear_buffer();

   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar, tam_registro);
   cargar_int_al_buffer(buffer_a_enviar, dir_fisica);

   t_paquete *paquete = crear_paquete(MEMORIA_MOV_IN, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
   
   op_code operacion = recibir_operacion(fd_memoria);
   t_buffer *buffer = crear_buffer();
   if(operacion == MEMORIA_ERROR){ // Antes iba un while
      log_error(logger_cpu, "Error, no se pudo leer en el PID: %i direccion fisica: %i\n ", EXEC->pid, dir_fisica);
      buffer = recibir_buffer_completo(fd_memoria);
      destruir_buffer(buffer);
      return 0;
   }
   
   buffer = recibir_buffer_completo(fd_memoria);
   void *datos = extraer_datos_del_buffer(buffer);

   // ES UN PRINT PARA VER LOS DATOS RECIBIDOS EN BYTES POR EL VOID*
   printf("\n##### DATOS A RECIBIDOS DEL MOV_IN: #####\n");
   unsigned char* byte_datos = (unsigned char*)datos;
   for(int i = 0; i < tam_registro; i++){
      printf("byte %d: %02X\n", i, byte_datos[i]);
   }
   printf("\n");
   // --------------------------------------------------------------

   int datos_a_int;
   memcpy(&datos_a_int, byte_datos, sizeof(int)); // Transforma lo obtenido por el void* en int para poder almacenarlo en el registro correspondiente

   // LOG OBLIGATORIO
   log_info(logger_cpu, "Acceso a espacio de usuario: \"PID: %d - Acción LEER - Direccion Física %d - Valor: %d\"", EXEC->pid, dir_fisica, datos_a_int);

   // guardo los datos recibidos en el registro indicado
   ejecutar_set(registro_datos, datos_a_int); // ANTES ESTABA registro_direccion y &datos
   log_info(logger_cpu, "Se realizo correctamente el MOV_IN");

   destruir_buffer(buffer);
   free(datos);
   return 0;
}

/*
   Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida a
   partir de la Dirección Lógica almacenada en el Registro Dirección.
*/
void ejecutar_mov_out(char *registro_direccion, char *registro_datos){

   printf("Registro Datos %s - Registro Dirección %s\n", registro_datos, registro_direccion);

   int datos_escribir = get_registro(registro_datos); // Copio el valor que hay en registro datos
   int dir_logica = get_registro(registro_direccion); // Obtengo la dir logica segun el registro

   // obtengo  la dir fisica
   //t_dir_fisica* dir_fisica = traducir_direccion_logica(dir_logica); // NUEVO DE LUCHO - AGREGAR

   uint32_t dir_fisica = traducir_direccion_logica(dir_logica); // VIEJO DE NACHO - BORRAR

   // LOG OBLIGATORIO
   log_info(logger_cpu, "Acceso a espacio de usuario: \"PID: %d - Acción ESCRIBIR - Direccion Física %d - Valor: %d\"", EXEC->pid, dir_fisica, datos_escribir);
   
   int tam_registro = get_tamanio_registro(registro_datos); // Agregamos el tamanio para el memcpy en memoria
   // void* data = malloc(tam_registro);
   // memmove(data, &datos_escribir, sizeof(datos_escribir));

   printf("\n### Envia el MOV_OUT -> Datos a escribir en memoria: %i #####\n\n", datos_escribir);

   // envio todo el paquete a escribir
   t_buffer *buffer_a_enviar = crear_buffer();
   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar, tam_registro);
   //cargar_int_al_buffer(buffer_a_enviar, dir_fisica->offset); // NUEVO DE LUCHO - AGREGAR
   cargar_int_al_buffer(buffer_a_enviar, dir_fisica); // VIEJO DE NACHO - BORRAR
   cargar_int_al_buffer(buffer_a_enviar, datos_escribir); // lo trato como void* por ahora creo es lo mas conveniente //ACA ESTA EL PROBLEMA CREO
   //cargar_datos_al_buffer(buffer_a_enviar, data, sizeof(datos_escribir));

   t_paquete *paquete = crear_paquete(MEMORIA_MOV_OUT, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
   //free(dir_fisica); // NUEVO DE LUCHO - AGREGAR
}

void ejecutar_io_gen_sleep(char *interfaz, char *unidades_de_trabajo){
   // IO_GEN_SLEEP (Interfaz, Unidades de trabajo):
   // Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.instruccion
 

   t_buffer *buffer= crear_buffer();
   t_paquete *paquete = crear_paquete(IO_GEN_SLEEP_FS, buffer);

   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);  
             // interfaz io  que debe hacer el sleep
   cargar_int_al_buffer(paquete->buffer, atoi(unidades_de_trabajo)); // tiempo en ms de sleep
   
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_stdin_read(char *interfaz, char *reg_direccion, char *reg_tamanio){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz ingresada se lea desde el
    STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño
     y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.

   */

   int dir_logica = get_registro(reg_direccion);
   int dir_fisica = (int) traducir_direccion_logica(dir_logica);
   int tamanio = get_registro(reg_tamanio);
   // envio todo el paquete de lo que necesito leer
   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_STDIN_READ_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);

   //cargar_int_al_buffer(paquete->buffer, dir_logica);
   cargar_int_al_buffer(paquete->buffer, dir_fisica);
   cargar_int_al_buffer(paquete->buffer, tamanio);

   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);

}

void ejecutar_io_stdout_write(char *interfaz, char *reg_direccion, char *reg_tamanio){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde la
    posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección,
    un tamaño indicado por el Registro Tamaño y se imprima por pantalla.

   */
   int dir_logica = get_registro(reg_direccion);
   int dir_fisica = traducir_direccion_logica(dir_logica); //En realidad es fisica
   int tamanio = get_registro(reg_tamanio);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_STDOUT_WRITE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_int_al_buffer(paquete->buffer, dir_fisica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_create(char *interfaz, char *nombre_archivo){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se cree un archivo en el FS montado en dicha interfaz.
   */
   t_buffer *buffer = crear_buffer();   
   t_paquete *paquete = crear_paquete(IO_FS_CREATE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_delete(char *interfaz, char *nombre_archivo){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se elimine un archivo en el FS montado en dicha interfaz
   */
   t_buffer *buffer = crear_buffer();   
   t_paquete *paquete = crear_paquete(IO_FS_DELETE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}
//IO_FS_TRUNCATE Int4 notas.txt ECX

void ejecutar_io_fs_truncate(char *interfaz, char *nombre_archivo, char *reg_tamanio){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se modifique el tamaño del archivo en el FS montado en dicha interfaz,
   actualizando al valor que se encuentra en el registro indicado por Registro Tamaño.
   */
   int tamanio = get_registro(reg_tamanio);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_FS_TRUNCATE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_write(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la dirección lógica
   que se encuentra en el Registro Dirección y se escriban en el archivo a partir del valor del Registro Puntero Archivo.
   */
   int dir_logica = get_registro(reg_direccion);
   int dir_fisica = (int) traducir_direccion_logica(dir_logica);
   int tamanio = get_registro(reg_tamanio);
   int puntero_archivo = get_registro(reg_puntero_archivo);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_FS_WRITE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   cargar_int_al_buffer(paquete->buffer, dir_fisica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   cargar_int_al_buffer(paquete->buffer, puntero_archivo);

   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_read(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo){
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se lea desde el archivo a partir del valor del Registro Puntero Archivo la cantidad de bytes indicada
   por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica indicada en el Registro Dirección.

   */
   int dir_logica = get_registro(reg_direccion);
   int dir_fisica = (int) traducir_direccion_logica(dir_logica);
   int tamanio = get_registro(reg_tamanio);
   int puntero_archivo = get_registro(reg_puntero_archivo);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_FS_READ_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   cargar_int_al_buffer(paquete->buffer, dir_fisica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   cargar_int_al_buffer(paquete->buffer, puntero_archivo);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_exit(){
   log_info(logger_cpu, "EJECUTANDO EXIT");
   t_buffer *buffer_a_enviar = crear_buffer();
   t_paquete *paquete = crear_paquete(KERNEL_EXIT, buffer_a_enviar);
   agregar_pcb(paquete,EXEC);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

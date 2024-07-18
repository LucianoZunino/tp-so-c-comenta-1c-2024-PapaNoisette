
#include "instrucciones.h"



void ejecutar_set(char *registro, int valor)
{
      log_info(logger_cpu,"ejecutando set,  valor: %d, registro destino %s\n", valor,registro);

   if (strcmp(registro, "AX") == 0)
   {
      EXEC->registros_cpu->AX = valor;
   }
   else if (strcmp(registro, "BX") == 0)
   {
      EXEC->registros_cpu->BX = valor;
   }
   else if (strcmp(registro, "CX") == 0)
   {
      EXEC->registros_cpu->CX = valor;
   }
   else if (strcmp(registro, "DX") == 0)
   {
      EXEC->registros_cpu->DX = valor;
   }
   else if (strcmp(registro, "EAX") == 0)
   {
      EXEC->registros_cpu->EAX = valor;
   }
   else if (strcmp(registro, "EBX") == 0)
   {
      EXEC->registros_cpu->EBX = valor;
   }
   else if (strcmp(registro, "ECX") == 0)
   {
      EXEC->registros_cpu->ECX = valor;
   }
   else if (strcmp(registro, "EDX") == 0)
   {
      EXEC->registros_cpu->EDX = valor;
   }
   else if (strcmp(registro, "SI") == 0)
   {
      EXEC->registros_cpu->SI = valor;
   }
   else if (strcmp(registro, "DI") == 0)
   {
      EXEC->registros_cpu->DI = valor;
   }
   else if (strcmp(registro, "PC") == 0)
   {
      EXEC->program_counter = valor;
   }
   else
   {
      log_error(logger_cpu, "El argumento de la instrucción SET es incorrecto");
   }
}

void ejecutar_sum(char *registro_destino, char *registro_origen)
{
   int sum = get_registro(registro_destino) + get_registro(registro_origen);

   ejecutar_set(registro_destino, sum);
}
void ejecutar_sub(char *registro_destino, char *registro_origen)
{
   int sub = get_registro(registro_destino) - get_registro(registro_origen);
   ejecutar_set(registro_destino, sub);
}
void ejecutar_jnz(char *registro, char *numero_instruccion)
{
   if (get_registro(registro) != 0)
   {
      ejecutar_set(PROGRAM_COUNTER, numero_instruccion);
   }
}

int ejecutar_resize(char *tamanio)
{
   /*Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro.
    En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto
    de ejecución al Kernel informando de esta situación.*/


   t_buffer *buffer_a_enviar = crear_buffer();

   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);  // Proceso al que se le reajustara su tamaño en memoria
   cargar_int_al_buffer(buffer_a_enviar, atoi(tamanio)); // nuevo tamaño del proceso

   t_paquete *paquete = crear_paquete(MEMORIA_RESIZE, buffer_a_enviar);
   log_info(logger_cpu, " ENVIANDO RESIZE A MEMORIA pid %d size: %d\n",EXEC->pid,atoi(tamanio));

   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);

//--tiempo de retardo establecido 
   t_buffer *buffer = crear_buffer();
   log_info(logger_cpu, " ESPERANDO RESPUESTA\n");
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
void ejecutar_copy_string(char *tamanio)
{
   int dir_fisica_si = traducir_direccion_logica(EXEC->registros_cpu->SI);
   int dir_fisica_di = traducir_direccion_logica(EXEC->registros_cpu->DI);

   log_info(logger_cpu, " Ejecutando  COPY_STRING");
   t_buffer *buffer_a_enviar_leer = crear_buffer();
   cargar_int_al_buffer(buffer_a_enviar_leer, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar_leer, tamanio);

   cargar_int_al_buffer(buffer_a_enviar_leer, dir_fisica_si);
   cargar_int_al_buffer(buffer_a_enviar_leer, dir_fisica_di);

   t_paquete *paquete_leer = crear_paquete(MEMORIA_COPY_STRING, buffer_a_enviar_leer);
   enviar_paquete(paquete_leer, fd_memoria);
   eliminar_paquete(paquete_leer);

}

void ejecutar_wait(char *recurso)
{
   log_info(logger_cpu, " ENVIANDO WAIT A KERNEL");

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(KERNEL_WAIT, buffer);
  //devuelvo contexto por solicitar llamada a kernel
   agregar_pcb (paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, recurso);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}
void ejecutar_signal(char *recurso)
{
   log_info(logger_cpu, " ENVIANDO SIGNAL A KERNEL");

  //devuelvo contexto por solicitar llamada a kernel

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(KERNEL_SIGNAL, buffer);
   agregar_pcb (paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, recurso);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_mov_in(char *registro_datos, char *registro_direccion)
{

   /*
    Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro
    Dirección y lo almacena en el Registro Datos.

   */
   log_info(logger_cpu, " ENVIANDO MOV IN A MEMORIA");
   int dir_logica = -1;
   // obtengo la dir logica
   dir_logica = get_registro(registro_direccion);
   //calculo el tamaño a leer en memoria 
   int tam_registro =get_tamanio_registro(registro_datos);
   // obtengo  la dir fisica
   int dir_fisica = traducir_direccion_logica(dir_logica);

   // envio todo el paquete a escribir
   t_buffer *buffer_a_enviar = crear_buffer();

   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar, dir_fisica);

   t_paquete *paquete = crear_paquete(MEMORIA_MOV_IN, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
   while (recibir_operacion(fd_memoria) != MOV_IN)
   {
      log_info(logger_cpu, "Esperando repuesta a MOV_IN ");
   }
   t_buffer *buffer = crear_buffer();
   buffer = recibir_buffer_completo(fd_memoria);
   void *datos = extraer_datos_del_buffer(buffer);

   // LOG OBLIGATORIO
   log_info(logger_cpu, "PID: %d -ACCION LEER - Direccion Fisica %d- Valor: %s", EXEC->pid, dir_fisica, datos);
   // guardo los datos recibidos en el registro indicado
   ejecutar_set(registro_direccion, datos);
   log_info(logger_cpu, "Se realizo correctamente el MOV_IN");
   return 0;
}

void ejecutar_mov_out(char *registro_direccion, char *registro_datos ) // done, testear
{

   /*
    Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida a
    partir de la Dirección Lógica almacenada en el Registro Dirección.

   */printf("flag  registro_datos %s-registro_direccion %s\n",registro_datos,registro_direccion);

   log_info(logger_cpu, " ENVIANDO MOV OUT A MEMORIA");
printf("flag  movout1\n");
   // me copio el valor que hay en registro datos
   long int datos_escribir = get_registro(registro_datos);
printf("flag  movout2\n");

   // obtengo la dir logica segun el registro
   uint32_t dir_logica;
   dir_logica = get_registro(registro_direccion);
printf("flag  movout33 dir_logica%d\n",dir_logica);
printf("flag  movout33 datos_escribir%d\n",datos_escribir);

   // obtengo  la dir fisica
   uint32_t dir_fisica = traducir_direccion_logica(dir_logica);
   printf("flag  movout33 dir_fisica%d\n",dir_fisica);

printf("flag  movout41\n");

   // LOG OBLIGATORIO
   log_info(logger_cpu, "PID: %d -ACCION ESCRIBIR - Direccion Fisica %d- Valor: %d", EXEC->pid, dir_fisica, datos_escribir);

   // envio todo el paquete a escribir
   t_buffer *buffer_a_enviar = crear_buffer();
   cargar_int_al_buffer(buffer_a_enviar, EXEC->pid);
   cargar_int_al_buffer(buffer_a_enviar, dir_fisica);
   printf("flag  movout5\n");

   cargar_datos_al_buffer(buffer_a_enviar, datos_escribir,strlen(datos_escribir)); // lo trato como void* por ahora creo es lo mas conveniente
   t_paquete *paquete = crear_paquete(MEMORIA_MOV_OUT, buffer_a_enviar);
   enviar_paquete(paquete, fd_memoria);
   eliminar_paquete(paquete);
}

void ejecutar_io_gen_sleep(char *interfaz, char *unidades_de_trabajo)
{
   // IO_GEN_SLEEP (Interfaz, Unidades de trabajo):
   // Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.instruccion
     printf("ejecutar_io_gen_sleep  \n");

     printf("flag1 \n");
   log_info(logger_cpu, " ENVIANDO solicitud de sleep A KERNEL");

   t_buffer *buffer= crear_buffer();
   t_paquete *paquete = crear_paquete(IO_GEN_SLEEP_FS, buffer);

   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);  
             // interfaz io  que debe hacer el sleep
   cargar_int_al_buffer(paquete->buffer, atoi(unidades_de_trabajo)); // tiempo en ms de sleep
   
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_stdin_read(char *interfaz, char *reg_direccion, char *reg_tamanio)
{
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz ingresada se lea desde el
    STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño
     y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.

   */

   int dir_logica = get_registro(reg_direccion);
   int tamanio = get_registro(reg_tamanio);
   // envio todo el paquete de lo que necesito leer
   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_STDIN_READ_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);

   cargar_int_al_buffer(paquete->buffer, dir_logica);
      cargar_int_al_buffer(paquete->buffer, tamanio);

   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);

}
void ejecutar_io_stdout_write(char *interfaz, char *reg_direccion, char *reg_tamanio)
{
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde la
    posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección,
    un tamaño indicado por el Registro Tamaño y se imprima por pantalla.

   */
   int dir_logica = get_registro(reg_direccion);
   int tamanio = get_registro(reg_tamanio);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_STDOUT_WRITE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_int_al_buffer(paquete->buffer, dir_logica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_create(char *interfaz, char *nombre_archivo)
{
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

void ejecutar_io_fs_delete(char *interfaz, char *nombre_archivo)
{
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

void ejecutar_io_fs_truncate(char *interfaz, char *nombre_archivo, char *reg_tamanio)
{
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
   
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}


void ejecutar_io_fs_write(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo)
{
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la dirección lógica
   que se encuentra en el Registro Dirección y se escriban en el archivo a partir del valor del Registro Puntero Archivo.
   */
   int dir_logica = get_registro(reg_direccion);
   int tamanio = get_registro(reg_tamanio);
   int puntero_archivo = get_registro(reg_puntero_archivo);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_FS_WRITE_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   cargar_int_al_buffer(paquete->buffer, dir_logica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   cargar_int_al_buffer(paquete->buffer, puntero_archivo);

   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}

void ejecutar_io_fs_read(char *interfaz, char *nombre_archivo, char *reg_direccion, char *reg_tamanio, char *reg_puntero_archivo)
{
   /*
   Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
   se lea desde el archivo a partir del valor del Registro Puntero Archivo la cantidad de bytes indicada
   por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica indicada en el Registro Dirección.

   */
   int dir_logica = get_registro(reg_direccion);
   int tamanio = get_registro(reg_tamanio);
   int puntero_archivo = get_registro(reg_puntero_archivo);

   t_buffer *buffer = crear_buffer();
   t_paquete *paquete = crear_paquete(IO_FS_READ_FS, buffer);
   agregar_pcb(paquete,EXEC);
   cargar_string_al_buffer(paquete->buffer, interfaz);
   cargar_string_al_buffer(paquete->buffer, nombre_archivo);
   cargar_int_al_buffer(paquete->buffer, dir_logica);
   cargar_int_al_buffer(paquete->buffer, tamanio);
   cargar_int_al_buffer(paquete->buffer, puntero_archivo);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);
}


void  ejecutar_exit(){
      log_info(logger_cpu, "EJECUTANO EXIT");
      t_buffer *buffer_a_enviar = crear_buffer();
 t_paquete *paquete = crear_paquete(KERNEL_EXIT, buffer_a_enviar);
    agregar_pcb(paquete,EXEC);
   enviar_paquete(paquete, fd_kernel_dispatch);
   eliminar_paquete(paquete);

}

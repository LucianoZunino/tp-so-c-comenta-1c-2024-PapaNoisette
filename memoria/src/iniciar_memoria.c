#include "iniciar_memoria.h"

t_config *config_memoria;
int cantidad_de_marcos;
t_list *lista_de_tablas_de_paginas_por_proceso;
t_list* lista_de_interfaces;
pthread_mutex_t *mutex_instrucciones_proceso;
void *memoria_RAM; // Memoria contigua
t_list *lista_de_instrucciones_por_proceso;
t_list *lista_de_frames;
t_list *lista_procesos;

void iniciar_memoria(){
    iniciar_logger_memoria();
    iniciar_config_memoria();
    inicializar_estructuras_memoria();
}

void inicializar_estructuras_memoria(){
    lista_de_instrucciones_por_proceso = list_create();
    lista_de_frames = list_create();
    lista_procesos = list_create();

    // Asignacion de memoria contigua
    memoria_RAM = malloc(tam_memoria);
    if(memoria_RAM == NULL){
        log_error(logger_memoria, "Error al asignar memoria\n");
        return EXIT_FAILURE;
    }
    memset(memoria_RAM, 0, tam_memoria); // Poner todo el espacio asignado en 0

    // Inicializar bitmap de frames
    for(int i = 0; i < cantidad_de_marcos; i++){
        Frame *frame = malloc(sizeof(Frame));
        frame->ocupado = 0;
        frame->pid = -1;
        list_add(lista_de_frames, frame);
    }
}

void inicializar_semaforos(){
    if(pthread_mutex_init(&mutex_instrucciones_proceso, NULL) != 0){
        log_error(logger_memoria, "No se pudo inicializar el semaforo para la lista lista_de_instrucciones_por_proceso");
        exit(-1);
    }
}

void iniciar_logger_memoria(){
    logger_memoria = iniciar_logger("memoria.log", "Memoria");
}

void imprimir_config_memoria(){
    printf("\n============================================================\n");
    printf("Puerto de escucha: %s\n", puerto_escucha);
    printf("Tamaño de la memoria RAM: %d\n", tam_memoria);
    printf("Tamaño de página: %d\n", tam_pagina);
    printf("Path de instrucciones: %s\n", path_instrucciones);
    printf("Retardo de respuesta: %d\n", retardo_respuesta);
    printf("Cantidad de marcos (Frames): %d\n", cantidad_de_marcos);
    printf("============================================================\n\n");
}

void iniciar_config_memoria(){
    config_memoria = iniciar_config("memoria.config");
    puerto_escucha = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config_memoria, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config_memoria, "TAM_PAGINA");
    path_instrucciones = config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");
    retardo_respuesta = config_get_int_value(config_memoria, "RETARDO_RESPUESTA");
    cantidad_de_marcos = tam_memoria / tam_pagina; // 4096 / 32 = 128
    imprimir_config_memoria();
}

void finalizar_memoria(){
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
    eliminar_estructuras_memoria();
}

void eliminar_estructuras_memoria(){
    /*
    list_destroy_and_destroy_elements(lista_de_miniPcb,);
    list_destroy_and_destroy_elements(lista_de_frames,);
    list_destroy_and_destroy_elements(lista_procesos,);
    list_destroy_and_destroy_elements(lista_de_frames, free);
    list_destroy_and_destroy_elements(lista_procesos, free);
    free(memoria_RAM);
    */
}

/// @brief Crea un proceso para un pid
/// @note Crea una estructura Proceso para un nuevo proceso,
//        inicializa su tabla de páginas y devuelve un puntero a esta estructura
/// @param pid, numero_paginas
/// @return  Proceso
void memoria_crear_proceso(int pid){
    // inicio la Proceso
    Proceso *proceso = (Proceso *)malloc(sizeof(Proceso)); // Se le asigna un espacio de memoria a esa tabla

    if(proceso == NULL){
        log_error(logger_memoria, "Error al asignar memoria para el proceso\n");
        return NULL;
    }
    proceso->pid = pid;

    // inicio la tabla de paginas

    proceso->tabla_paginas = list_create();

    if(proceso->tabla_paginas == NULL){
        log_error(logger_memoria, "Error al asignar memoria para las tabla paginas\n");
        exit(EXIT_FAILURE);
    }

    printf("proceso->tabla_paginas  list size%d\n", list_size(proceso->tabla_paginas));
    printf("PID: %d - Tabla de páginas creada con 0 páginas\n", pid);
    list_add(lista_procesos, proceso);
    print_lista_de_frames("lista_de_frames_inicializado.txt");
    print_lista_procesos("lista_procesos_inicializado.txt");
    print_memoria_RAM("contenido_memoria_RAM_inicializada.txt");
}

/// @brief  Redimensiona la tábla de páginas del proceso
/// @note Crea una estructura Proceso para un nuevo proceso,
//        inicializa su tabla de páginas y devuelve un puntero a esta estructura
/// @param pid, numero_paginas
/// @return  Proceso
int resize_tamano_proceso(int pid, int nuevo_tamano){

    Proceso *proceso = buscar_proceso(lista_procesos, pid);
    printf("flag proceso\n");
    if(proceso->tabla_paginas == NULL){
        log_error(logger_memoria, "No se creo correctamente la tabla de paginas\n");
        exit(EXIT_FAILURE);
    }
    int numero_paginas_actual = list_size(proceso->tabla_paginas);
    printf("flag numero_paginas_actual %d\n", numero_paginas_actual);

    // Calcula el número de páginas necesarias
    //(La cuenta rara es por si el proceso necesita 1 página y 1byte mas de otra necesitaria 2 páginas)
    int numero_paginas_nuevo = (nuevo_tamano + tam_pagina - 1) / tam_pagina;

    if(numero_paginas_nuevo < 0){ // no deberia de suceder ,pero por las dudas
        numero_paginas_nuevo = 0;
    }

    int paginas_a_agregar_o_quitar = numero_paginas_nuevo - numero_paginas_actual;
    int frames_disponibles = contar_frames_libres();
    printf(" numero_paginas_nuevo %d\n", numero_paginas_nuevo);

    if(numero_paginas_nuevo > numero_paginas_actual){

        // Ampliación de un proceso
        log_info(logger_memoria, "Ampliación de Proceso PID: %d -Tamaño Actual:%d - Tamaño a Ampliar: %d", proceso->pid, numero_paginas_actual * tam_pagina, nuevo_tamano);

        if(frames_disponibles >= paginas_a_agregar_o_quitar){

            for(int i = 0; i < paginas_a_agregar_o_quitar; i++){

                int nueva_pagina;

                //nueva_pagina->bit_presencia = 1;
                nueva_pagina = asignar_y_marcar_frame_ocupado(proceso->pid);
                list_add(proceso->tabla_paginas, nueva_pagina); // Agrega numero de pagina a la tabla de paginas del proceso
                // printf("agrego nueva pagina al pid %d bit %d nuevo marco %d \n", proceso->pid,nueva_pagina->bit_presencia,nueva_pagina->numero_de_marco);
                // free(nueva_pagina);
            }

            printf("PID: %d - Redimensionado a %d páginas\n", proceso->pid, numero_paginas_nuevo);
            print_lista_de_frames("lista_de_frames_resize_ampliar.txt");
            print_lista_procesos("lista_de_procesos_resize_ampliar.txt");
        }
        else{
         // OUT_OF_MEMORY

            log_error(logger_memoria, "Error: No hay suficiente memoria para redimensionar el proceso (OUT_MEMORY)\n");
            return -1; // Error OUT_OF_MEMORY
        }

    } // Reducción de un proceso
    else if(numero_paginas_nuevo < numero_paginas_actual){

        log_info(logger_memoria, "Reduccion del Proceso PID: %d -Tamaño Actual:%d - Tamaño a reducir: %d", proceso->pid, numero_paginas_actual * tam_pagina, nuevo_tamano);
        printf("numero_paginas_actual%d\n", numero_paginas_actual);
        int pagina;
        for(int i = numero_paginas_nuevo; i < numero_paginas_actual; i++){
            printf("list_get %d list_size %d\n", i, list_size(proceso->tabla_paginas));
            pagina = list_get(proceso->tabla_paginas, list_size(proceso->tabla_paginas) - 1); // el size -1 es para que saque el ultimo de la lista
            printf("liberar_frame\n");

            liberar_frame(pagina);
            printf("liberar_frame done\n");

            printf("list_remove\n");
            list_remove(proceso->tabla_paginas, list_size(proceso->tabla_paginas) - 1); // el size -1 es para que saque el ultimo de la lista
            printf("list_remove done \n");
            free(pagina);
        }

        printf("PID: %d - Redimensionado a %d páginas\n", proceso->pid, numero_paginas_nuevo);
        print_lista_de_frames("lista_de_frames_resize_reducir.txt");
        print_lista_procesos("lista_de_procesos_resize_reducir.txt");
    }

    return 0;
}

/// @brief Cuenta los frames libres que tiene disponible memoria para todos los procesos
/// @return  cantidad_disponible
int contar_frames_libres(){
    int cantidad_disponible = 0;
    for(int i = 0; i < cantidad_de_marcos; i++){
        Frame *frame = list_get(lista_de_frames, i);
        if(!frame->ocupado){
            cantidad_disponible++;
        }
    }
    return cantidad_disponible;
}

int asignar_y_marcar_frame_ocupado(int pid){
    for(int i = 0; i < cantidad_de_marcos; i++){
        Frame *frame = list_get(lista_de_frames, i);

        if(!frame->ocupado){
            frame->ocupado = 1;
            frame->pid = pid;

            list_replace(lista_de_frames, i, frame); // remplazo en el bitmap de frames

            return i;
        }
    }

    return -1; // No hay frames libres, no deberia suceder, ya que verificamos previamente que existan frames libres
}

void liberar_frame(int numero_de_marco){
    if(numero_de_marco >= 0 && numero_de_marco < cantidad_de_marcos){
        Frame *frame = list_get(lista_de_frames, numero_de_marco);
        frame->ocupado = 0;
        frame->pid = -1; // cargamos asi los frames libres

        list_replace(lista_de_frames, numero_de_marco, frame);
        log_info(logger_memoria, "Se libero el frame :%d\n", numero_de_marco);
    }
}

void print_memoria_RAM(char *path_log){
    FILE *archivo = fopen(path_log, "w");
    int numero_frame = 0;

    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return;
    }

    fprintf(archivo, "Contenido de la memoria RAM:\n\n");
    fprintf(archivo, "------------------ Frame %d -------------------\n", numero_frame);

    for(int i = 0; i < tam_memoria; i++){
        fprintf(archivo, "%02X ", ((unsigned char *)memoria_RAM)[i]);
        if((i + 1) % 16 == 0){
            fprintf(archivo, "\n");
        }
        if((i + 1) % 32 == 0 && (i + 1) != tam_memoria){
            numero_frame++;
            fprintf(archivo, "------------------ Frame %d -------------------\n", numero_frame);
        }
    }

    fclose(archivo);
}

void print_lista_de_frames(char *path_log){
    FILE *archivo = fopen(path_log, "w");

    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }

    fprintf(archivo, "Print lista de frames (%d frames)\n", cantidad_de_marcos);
    int num_frame = 0;
    int _closure_print_frame(Frame * frame){
        fprintf(archivo, "Frame %d ,Pid %d, Ocupado %d\n", num_frame, frame->pid, frame->ocupado);
        num_frame++;
    }
    list_iterate(lista_de_frames, _closure_print_frame);

    // Cierra el archivo
    fclose(archivo);
}

void print_lista_procesos(char *path_log){
    FILE *archivo = fopen(path_log, "w");
    if(archivo == NULL){
        printf("Error al abrir el archivo\n");
        return 1;
    }
    int cant_procesos = list_size(lista_procesos);
    fprintf(archivo, "Print lista_procesos (%d procesos)\n\n", cant_procesos);

    for(int i = 0; i < cant_procesos; i++){ // itero todos los procesos
        Proceso *proceso = list_get(lista_procesos, i);
        fprintf(archivo, "{Proceso: %d, cantidad de paginas: %d}\n", proceso->pid, list_size(proceso->tabla_paginas));

        for(int j = 0; j < list_size(proceso->tabla_paginas); j++){
            int frame = list_get(proceso->tabla_paginas, j); 

            fprintf(archivo, "\tPagina %d -> Frame %d\n", j, frame);
        }
    }

    fclose(archivo);
}

Proceso *buscar_proceso(t_list *lista, int pid){
    // REVISAR SI RECIBE T_LIST
    for(unsigned int i = 0; i < list_size(lista); i++){
        Proceso *proceso = list_get(lista, i);
        if(proceso->pid == pid){
            return proceso;
        }
    }
    return NULL;
}

void finalizar_proceso(int pid){
    for(int i = 0; i < cantidad_de_marcos; i++){
        Frame *frame = list_get(lista_de_frames, i);

        if(frame->pid == pid){
            frame->ocupado = 0;
            list_replace(lista_de_frames, i, frame);
            log_info(logger_memoria, "Se libero el frame :%d\n", i);
        }
    }
    log_info(logger_memoria, "Se finalizo el proceso :%d", pid);
}

void leer_memoria(int marco, void *buffer, size_t tamano){
    memcpy(buffer, memoria_RAM + (marco * tam_pagina), tamano);
}

void escribir_memoria(int marco, void *buffer, size_t tamano){ //mov out
    memcpy(memoria_RAM + (marco * tam_pagina), buffer, tamano);
}

void esperar_clientes(){
    int i = 0;
    lista_de_interfaces = list_create();

    while(1){
        int socket;// = malloc(sizeof(int));  //LIBERAR MEMORIA CUANDO SE DESCONECTE IO
        socket = esperar_cliente(fd_memoria, logger_memoria, "entradasalida");
        printf("Socket: %i, despues de esperar_cliente \n", socket);
        if (socket == -1) return;
        t_interfaz* interfaz = malloc(sizeof(t_interfaz));
        
        interfaz->socket = socket;
        interfaz->cola_espera = list_create();
        sem_init(&interfaz->sem_espera, 1, 0);
        pthread_mutex_init(&interfaz->mutex_interfaz, NULL);

        list_add(lista_de_interfaces, interfaz);
        printf("tam lista_de_interfaces: %i\n", list_size(lista_de_interfaces));
 
        pthread_t hilo_interfaz;
        pthread_create(&hilo_interfaz, NULL, (void *) escuchar_mensajes_entradasalida_memoria, i);
        pthread_detach(hilo_interfaz);
        printf("FIN DEL WHILE esperar_clientes \n");
        
        i++;
    }
}

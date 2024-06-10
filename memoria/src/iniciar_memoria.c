#include "iniciar_memoria.h"

t_config* config_memoria;
t_list* lista_de_miniPcb;
pthread_mutex_t* mutex_miniPcb;

void iniciar_memoria(){
    iniciar_logger_memoria();
    iniciar_config_memoria();
    //imprimir_config_memoria();

    // Inicializa la tabla de páginas y el espacio de memoria
    //TablaDePaginas tabla_de_paginas;
    //inicializar_tabla_paginas(&tabla_de_paginas, numero_paginas);
    //void *memoria = asignar_memoria(tam_pagina * numero_paginas); // 32 * 128 = 4096
    
    // Ejemplo de asignación de páginas
    //asignar_marco(&tabla_de_paginas, 2, 5);
    /*
    // Dirección virtual a traducir (por ejemplo, byte 100 de la página 0)
    unsigned int virtual_address = 100;
    void *physical_address = translate(&page_table, memoria, virtual_address);

    if (physical_address != NULL) {
        printf("Dirección física: %p\n", physical_address);
    }*/

    // Limpia la memoria
    //free(tabla_de_paginas.pagina);
    //free(memoria);
}

void iniciar_logger_memoria(){
    logger_memoria = iniciar_logger("memoria.log", "Memoria");
}

void iniciar_config_memoria(){
    config_memoria = iniciar_config("memoria.config");
    puerto_escucha = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config_memoria, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config_memoria, "TAM_PAGINA");
    path_instrucciones = config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");
    retardo_respuesta = config_get_int_value(config_memoria, "RETARDO_RESPUESTA");
    numero_paginas = tam_memoria/tam_pagina; // 4096 / 32 = 128
}

// Función crear Proceso


// Función para inicializar la tabla de páginas
void inicializar_tabla_paginas(TablaDePaginas *tabla, int numero_paginas){
    tabla->pagina = (Pagina *)malloc(numero_paginas * sizeof(Pagina)); // Castea el puntero genérico que devuelve el malloc a un puntero de tipo Pagina *
    if(tabla->pagina == NULL){
        fprintf(stderr, "Error al asignar memoria para las páginas\n");
        exit(EXIT_FAILURE);
    }
    tabla->numero_paginas = numero_paginas;

    for(int i = 0; i < numero_paginas; i++){
        tabla->pagina[i].numero_de_marco = -1; // -1 indica que no está asignado a ningun marco
        tabla->pagina[i].bit_presencia = 0;
    }
}


// Crea una estructura TablaDePaginasPorProceso para un nuevo proceso, inicializa su tabla de páginas y devuelve un puntero a esta estructura
TablaDePaginasPorProceso *memoria_crear_proceso(int pid){
    TablaDePaginasPorProceso *tablaDePaginasDelProceso = (TablaDePaginasPorProceso *)malloc(sizeof(TablaDePaginasPorProceso)); // Se le asigna un espacio de memoria a esa tabla
    if(tablaDePaginasDelProceso == NULL){
        fprintf(stderr, "Error al asignar memoria para el proceso\n");
        return NULL;
    }
    tablaDePaginasDelProceso->pid = pid;
    tablaDePaginasDelProceso->tabla_paginas = (TablaDePaginas *)malloc(sizeof(TablaDePaginas));
    if(tablaDePaginasDelProceso->tabla_paginas == NULL){
        fprintf(stderr, "Error al asignar memoria para la tabla de páginas\n");
        free(tablaDePaginasDelProceso);
        return NULL;
    }
    inicializar_tabla_paginas(tablaDePaginasDelProceso->tabla_paginas, 0); // Tabla de páginas vacía inicialmente
    printf("PID: %d - Tabla de páginas creada\n", pid);
    return tablaDePaginasDelProceso;
}

void resize_tamano_proceso(TablaDePaginasPorProceso *proceso, int nuevo_tamano){
    int numero_paginas_actual = proceso->tabla_paginas->numero_paginas;
    int numero_paginas_nuevo = (nuevo_tamano + tam_pagina - 1) / tam_pagina; // tam_pagina es siempre 32

    if(numero_paginas_nuevo > numero_paginas_actual){
        // Ampliación de un proceso
        Pagina *nuevas_paginas = (Pagina *)realloc(proceso->tabla_paginas->pagina, numero_paginas_nuevo * sizeof(Pagina));
        if(nuevas_paginas == NULL){
            fprintf(stderr, "Error al redimensionar la tabla de páginas\n");
            // Opcional: Manejar error de memoria llena aquí (Out Of Memory)
            return;
        }
        proceso->tabla_paginas->pagina = nuevas_paginas;
        for(int i = numero_paginas_actual; i < numero_paginas_nuevo; i++){
            proceso->tabla_paginas->pagina[i].numero_de_marco = -1;
            proceso->tabla_paginas->pagina[i].bit_presencia = 0;
        }
        printf("PID: %d - Redimensionado a %d páginas\n", proceso->pid, numero_paginas_nuevo);
    }
    else if(numero_paginas_nuevo < numero_paginas_actual){
        // Reducción de un proceso
        for(int i = numero_paginas_nuevo; i < numero_paginas_actual; i++){
            // Opcional: Aquí puedes liberar marcos si es necesario
            proceso->tabla_paginas->pagina[i].numero_de_marco = -1;
            proceso->tabla_paginas->pagina[i].bit_presencia = 0;
        }
        Pagina *nuevas_paginas = (Pagina *)realloc(proceso->tabla_paginas->pagina, numero_paginas_nuevo * sizeof(Pagina));
        if(nuevas_paginas == NULL && numero_paginas_nuevo > 0){
            fprintf(stderr, "Error al redimensionar la tabla de páginas\n");
            return;
        }
        proceso->tabla_paginas->pagina = nuevas_paginas;
        printf("PID: %d - Redimensionado a %d páginas\n", proceso->pid, numero_paginas_nuevo);
    }

    proceso->tabla_paginas->numero_paginas = numero_paginas_nuevo;
}

// Función para asignar un marco a una página
void asignar_marco(TablaDePaginas *tabla, int numero_pagina, int numero_marco){
    if(numero_pagina >= tabla->numero_paginas){
        fprintf(stderr, "Número de página fuera de rango\n");
        return;
    }
    tabla->pagina[numero_pagina].numero_de_marco = numero_marco;
    tabla->pagina[numero_pagina].bit_presencia = 1;
    printf("Página %d asignada al marco %d\n", numero_pagina, numero_marco);
}

// Asignaciom de memoria contigua
void *asignar_memoria(size_t size){
    void *memoria = malloc(size);
    if(memoria == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(EXIT_FAILURE);
    }
    return memoria;
}

void imprimir_config_memoria(){
    printf("PUERTO_ESCUCHA:%s\n",puerto_escucha);
    printf("TAM_MEMORIA:%d\n",tam_memoria);
    printf("TAM_PAGINA:%d\n",tam_pagina);
    printf("PATH_INSTRUCCIONES:%s\n",path_instrucciones);
    printf("RETARDO_RESPUESTA:%d\n",retardo_respuesta);
}

void finalizar_memoria(){
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
}

void inicializar_estructuras(){
    t_list *lista_de_miniPcb = list_create();
}

void inicializar_semaforos(){
    if(pthread_mutex_init(&mutex_miniPcb, NULL) != 0){
        log_error(logger_memoria, "No se pudo inicializar el semaforo para la lista miniPcb");
        exit(-1);
    } 
}
#include "iniciar_kernel.h"

t_config* config_kernel;
t_list *NEW;
t_list *READY;
t_list *PRIORIDAD;
t_list *BLOCKED;
t_list *EXIT; 
// falta running
t_list *interfaces;

bool flag_planificacion;

sem_t sem_NEW;
sem_t sem_READY;
sem_t sem_MULTIPROGRAMACION;
sem_t sem_desalojo;
sem_t sem_EXIT;
sem_t sem_estructuras_inicializadas;
sem_t sem_planificador_LP_detenido;
sem_t sem_planificador_CP_detenido;
t_list* lista_de_estados;

void iniciar_kernel(){
    iniciar_logger_kernel();
    iniciar_config_kernel();
    iniciar_semaforos();
    iniciar_colas_estados();
    iniciar_recursos();
    //imprimir_config_kernel();
}

void iniciar_logger_kernel(){
    logger_kernel = iniciar_logger("kernel.log", "kernel");
}

void iniciar_config_kernel(){
    config_kernel = iniciar_config("kernel.config");

    puerto_escucha = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config_kernel, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config_kernel, "PUERTO_CPU_INTERRUPT");
    algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
    quantum = config_get_int_value(config_kernel, "QUANTUM");
    recursos = config_get_array_value(config_kernel, "RECURSOS");
    instancias_recursos = config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");
}

void imprimir_config_kernel(){
    printf("PUERTO_ESCUCHA: %s\n",puerto_escucha);
    printf("IP_MEMORIA: %s\n",ip_memoria);
    printf("PUERTO_MEMORIA: %s\n",puerto_memoria);
    printf("IP_CPU: %s\n",ip_cpu);
    printf("PUERTO_CPU_DISPATCH: %s\n",puerto_cpu_dispatch);
    printf("PUERTO_CPU_INTERRUPT: %s\n",puerto_cpu_interrupt);
    printf("ALGORITMO_PLANIFICACION: %s\n",algoritmo_planificacion);
    printf("QUANTUM: %d\n",quantum);
    for(int i=0; i<largo_array(recursos); i++){
        printf("RECURSOS: %s\n",recursos[i]);
    }
    for(int i=0; i<largo_array(instancias_recursos); i++){
        printf("INSTANCIAS_RECURSOS: %s\n",instancias_recursos[i]);

    }
    printf("GRADO_MULTIPROGRAMACION: %d\n",grado_multiprogramacion);
}

void iniciar_semaforos(){
    if (pthread_mutex_init(&mutex_NEW, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el semaforo para la cola de NEW");
        exit(-1);
    }
    printf("socket_memoria_mutex inicializado\n");
    if (pthread_mutex_init(&socket_memoria_mutex, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el socket_memoria_mutex");
        exit(-1);
    }
    printf("socket_memoria_mutex inicializado\n");
    if (pthread_mutex_init(&mutex_next_pid, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_next_pid");
        exit(-1);
    }
    if (sem_init(&sem_NEW, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_NEW");
        exit(-1);
    }
    if (sem_init(&sem_MULTIPROGRAMACION, 1, grado_multiprogramacion) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_MULTIPROGRAMACION");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_multiprogramacion, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_multiprogramacion");
        exit(-1);
    }
    if (sem_init(&sem_EXEC, 1, 1) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_EXEC");
        exit(-1);
    }
    if (sem_init(&sem_READY, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_READY, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_EXIT, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_PRIORIDAD, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_READY");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_BLOCKED, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_BLOCKED");
        exit(-1);
    }
    if (sem_init(&sem_desalojo, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_desalojo");
        exit(-1);
    }
    if (sem_init(&sem_EXIT, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_EXIT");
        exit(-1);
    }
    if (pthread_mutex_init(&mutex_recursos_disponibles, NULL) != 0) {
        log_error(logger_kernel, "No se pudo inicializar el mutex_recursos_disponibles");
        exit(-1);
    }
    if (sem_init(&sem_estructuras_inicializadas, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_estructuras_inicializadas");
        exit(-1);
    }
    if (sem_init(&sem_planificador_LP_detenido, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_planificador_LP_detenido");
        exit(-1);
    }
    if (sem_init(&sem_planificador_CP_detenido, 1, 0) != 0) {
        log_error(logger_kernel, "Ocurrio un error al crear semaforo sem_planificador_CP_detenido");
        exit(-1);
    }
}


void iniciar_colas_estados() {
    NEW = list_create();
    READY = list_create();
    PRIORIDAD = list_create();
    BLOCKED = list_create();
    EXIT = list_create();
    lista_de_estados = list_create();
    list_add(lista_de_estados, NEW);
    list_add(lista_de_estados, READY);
    list_add(lista_de_estados, BLOCKED);
    list_add(lista_de_estados, EXIT);
    list_add(lista_de_estados, PRIORIDAD);
}

void iniciar_recursos(){
    int length = sizeof(instancias_recursos) / sizeof(instancias_recursos[0]);

    t_list* recursos_disponibles = list_create();

    for (int i = 0; i < length; i++){
        t_recurso* nuevo_recurso = malloc (sizeof(t_recurso)); //HACER FREE CUANDO SE ELIMINA TODO
        char* nombre = recursos[i];
        int instancias = instancias_recursos[i];
        t_queue* cola_de_espera = queue_create();
        t_list* pcb_asignados = list_create();
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);

        nuevo_recurso->nombre = nombre;
        nuevo_recurso->instancias = instancias;
        nuevo_recurso->cola_de_espera = cola_de_espera;
        nuevo_recurso->pcb_asignados = pcb_asignados;
        nuevo_recurso->mutex = mutex;

        list_add(recursos_disponibles, nuevo_recurso);
    }

    flag_planificacion_detenido = 0;
}


void finalizar_kernel(){
    log_destroy(logger_kernel);
    // TODO: Destruir listas, hilos, etc
}


void esperar_clientes(){
    int i = 0;
    interfaces = list_create();
    printf("ENTRAMOS EN esperar_clientes \n");
    while(1){
        
        printf("antes de esperar_cliente \n");
        int socket;// = malloc(sizeof(int));  //LIBERAR MEMORIA CUANDO SE DESCONECTE IO
        socket = esperar_cliente(fd_kernel, logger_kernel, "entradasalida");
        printf("Socket: %i, despues de esperar_cliente \n", socket);
        if (socket == -1) return;
        t_interfaz* interfaz = malloc(sizeof(t_interfaz));
        
    
        interfaz->socket = socket;
        printf("Antes de crear el hilo\n");
        interfaz->cola_espera = list_create();
        sem_init(&interfaz->sem_espera, 1, 0);
        pthread_mutex_init(&interfaz->mutex_interfaz, NULL);

        list_add(interfaces, interfaz);
        printf("tam interfaces: %i\n", list_size(interfaces));

        pthread_t hilo_interfaz;
        pthread_create(&hilo_interfaz, NULL, (void *)(escuchar_mensajes_entradasalida_kernel), (i));
        pthread_detach(hilo_interfaz);
        printf("FIN DEL WHILE esperar_clientes \n");
        
        i++;
    }
}
#include "main.h"

t_log* logger_entradasalida;

int tipo_de_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
char* puerto_kernel;
char* ip_memoria;
char* puerto_memoria;
int block_size;
int block_count;
int retraso_compactacion;
char* path_base_dialfs;

int fd_memoria;
int fd_kernel;

int main(int argc, char* argv[]){

    iniciar_entradasalida(argv[2]); //argv[1]
    log_info(logger_entradasalida, "Arranca el modulo Entrada/Salida");

    // Se conecta como cliente a MEMORIA
    if(tipo_de_interfaz != GENERICA){
        fd_memoria = crear_conexion(ip_memoria, puerto_memoria, logger_entradasalida);
    }
  
    fd_kernel = crear_conexion(ip_kernel, puerto_kernel, logger_entradasalida); // Se conecta como cliente a KERNEL
   
    // if(realizar_handshake(logger_entradasalida, fd_kernel, HANDSHAKE_ENTRADASALIDA) == -1){
    //     return EXIT_FAILURE;
    // }

    // Escucha los mensajes Memoria-E/S
    if(tipo_de_interfaz != GENERICA){
        pthread_t hilo_memoria_entradasalida;
	    pthread_create(&hilo_memoria_entradasalida, NULL, escuchar_mensajes_memoria_entradasalida, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	    pthread_detach(hilo_memoria_entradasalida); // Hace que el hilo se desacople del principal y se ejecute en paralelo
    }
   
    //recibir_ok(fd_kernel); // Espera que kernel cree la conexion

    t_buffer *buffer = crear_buffer();
    cargar_string_al_buffer(buffer, argv[1]); // Ver orden parametros consola, argv[0] es el nombre de interfaz a crear
    t_paquete *paquete = crear_paquete(NUEVA_CONEXION_IO, buffer);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete);
    
    // Escucha los mensajes Kernel-E/S
    pthread_t hilo_kernel_entradasalida;
    switch(tipo_de_interfaz){
        case GENERICA:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_generica, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
        case STDIN:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_stdin, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
        case STDOUT:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_stdout, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
        case DIAL_FS:
            pthread_create(&hilo_kernel_entradasalida, NULL, escuchar_instrucciones_dialfs, NULL); // Crea el hilo y le pasa la funcion a ejecutarse
	        pthread_join(hilo_kernel_entradasalida, NULL);
            break;
    }
	// Frena el hilo principal hasta que el hilo_kernel_entradasalida no finalice
	// Porque si se el hilo_kernel_entradasalida se desacopla del principal termina el modulo Entradasalida
    finalizar_entradasalida(); // Finalizar Interfaz E/S

    return 0;
}

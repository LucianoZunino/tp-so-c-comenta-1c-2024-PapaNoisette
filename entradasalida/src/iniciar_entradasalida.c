#include "iniciar_entradasalida.h"
#include <dirent.h>
#include <fcntl.h> // Para open
#include <sys/stat.h> // Para los modos de apertura y permisos
#include <unistd.h> // Para close
#include <stdio.h> // Para perror

char* path_base_dialfs;

t_config* config_entradasalida;
sem_t sem_stdout;
sem_t sem_fs_write;
int fd_bloques;
int fd_bitmap;
void* bloques_dat;
t_bitarray* bitmap;
t_list* archivos_metadata;

void iniciar_entradasalida(char* path){
    printf("\n\nPATH QUE RECIBE DEL CONFIG: %s\n\n", path);
    iniciar_logger_entradasalida();
    iniciar_config_entradasalida(path);
    printf("flag post config IO\n");
    iniciar_estructuras();
    printf("Estructuras Inicializadas\n");
    imprimir_config_entradasalida();
    if(tipo_de_interfaz == DIAL_FS){
        iniciar_lista_metadatas();
    }
    imprimir_config_entradasalida();
}

void iniciar_logger_entradasalida(){
    logger_entradasalida = iniciar_logger("entradasalida.log", "entradasalida");
}

void iniciar_config_entradasalida(char* config){
    printf("INICIAR CONFIG ENTRADASALIDA \n");
    config_entradasalida = iniciar_config(config);
    printf("flag\n");
    ip_kernel = config_get_string_value(config_entradasalida, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config_entradasalida, "PUERTO_KERNEL");
    tipo_de_interfaz = leer_tipo_interfaz(config_entradasalida);
   // log_info(logger_auxiliar, "Tipo interfaz: %d", TIPO_INTERFAZ);
    switch(tipo_de_interfaz){
        case GENERICA:
            tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");            
            break;
        case STDIN:
            ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
            puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
            break;
        case STDOUT:
            tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
            ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
            puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
            break;
        case DIAL_FS:
            printf("\n\nENTRANDO AL CASE DIAL_FS\n\n");
            tiempo_unidad_trabajo = config_get_int_value(config_entradasalida, "TIEMPO_UNIDAD_TRABAJO");
            ip_memoria = config_get_string_value(config_entradasalida, "IP_MEMORIA");
            puerto_memoria = config_get_string_value(config_entradasalida, "PUERTO_MEMORIA");
            path_base_dialfs = config_get_string_value(config_entradasalida, "PATH_BASE_DIALFS");
            block_size = config_get_int_value(config_entradasalida, "BLOCK_SIZE");
            block_count = config_get_int_value(config_entradasalida, "BLOCK_COUNT");
            retraso_compactacion = config_get_int_value(config_entradasalida, "RETRASO_COMPACTACION");
            break;   
        }
}

void imprimir_config_entradasalida(){
    printf("TIPO_INTERFAZ: %i\n", tipo_de_interfaz);
    printf("TIEMPO_UNIDAD_TRABAJO: %d\n", tiempo_unidad_trabajo);
    printf("IP_KERNEL: %s\n", ip_kernel);
    printf("PUERTO_KERNEL: %s\n", puerto_kernel);
    printf("IP_MEMORIA: %s\n", ip_memoria);
    printf("PUERTO_MEMORIA: %s\n", puerto_memoria);
    printf("PATH_BASE_DIALFS: %s\n", path_base_dialfs);
    printf("BLOCK_SIZE: %d\n", block_size);
    printf("BLOCK_COUNT: %d\n", block_count);
    printf("RETRASO: %d\n", retraso_compactacion);
}


void iniciar_estructuras(){
    sem_init(&sem_stdout, 1, 0);
    sem_init(&sem_fs_write, 1, 0);

    if(tipo_de_interfaz == DIAL_FS){
        // Inicializacion bloques.dat
        char* path_bloques = tomar_nombre_devolver_path("bloques.dat");
        fd_bloques = open(path_bloques, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // O_RDWR | O_CREAT, S_IRUSR | S_IWUSR

        if(fd_bloques == -1){
            log_error(logger_entradasalida, "Error abriendo el archivo");
            close(fd_bloques);
            return EXIT_FAILURE;
        }

        ftruncate(fd_bloques, block_size * block_count);

        size_t length = lseek(fd_bloques, 0, SEEK_END);

        bloques_dat = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd_bloques, 0); // Mapeo del archivo de bloques en memoria. 
        if(bloques_dat == MAP_FAILED){
            perror("Error mapeando el archivo");
            close(fd_bloques);
            return EXIT_FAILURE;
        }
        
        // Inicializacion bitmap
        char* path_bitmap = tomar_nombre_devolver_path("bitmap.dat");
        fd_bitmap = open(path_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // O_RDWR | O_CREAT, S_IRUSR | S_IWUSR
        
        if(fd_bitmap == -1){
            log_error(logger_entradasalida, "Error abriendo el archivo");
            close(fd_bitmap);
            return EXIT_FAILURE;
        }

        ftruncate(fd_bitmap, block_count);
        
        void* bitarray = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd_bloques, 0); 
        
        bitmap = bitarray_create_with_mode(bitarray, redondear_up(block_count, 8), LSB_FIRST);
    }
}

void iniciar_lista_metadatas(){
    archivos_metadata = list_create();

    //char* path_bloques = tomar_nombre_devolver_path("bloques.dat");
	//strcpy(path_bloques, tomar_nombre_devolver_path("bloques.dat"));


    //char* path_bitmap = tomar_nombre_devolver_path("bitmap.dat");
    //strcpy(path_bitmap, tomar_nombre_devolver_path("bloques.dat"));
    DIR* directorio = opendir(path_base_dialfs);

    printf("\n\nPATH DE FS: %s\n\n", path_base_dialfs);

    
    if(directorio == NULL){
        log_error(logger_entradasalida, "No se pudo abrir el directorio %s", path_base_dialfs); //hacer algo mas?
    }

    struct dirent* entrada_directorio = readdir(directorio);
    printf("despues de readdir \n");
    while(entrada_directorio != NULL){
        if((strcmp(entrada_directorio->d_name, "bloques.dat") != 0 && strcmp(entrada_directorio->d_name, "bitmap.dat") != 0 &&
            strcmp(entrada_directorio->d_name, ".") != 0 && strcmp(entrada_directorio->d_name, "..") != 0 )){
            
            printf("\nDENTRO DEL IF DE lista_metadatas \n");
            list_add(archivos_metadata, entrada_directorio->d_name); //NOMBRE O PATH?
            printf("\nDESPUES DEL ADD \n");

        }
        entrada_directorio = readdir(directorio);
        printf("\nDESPUES DEL READDIR DEL WHILE \n");

    }
    printf("\nANTES DEL closedir \n");
    closedir(directorio);
    //free(path_bitmap);
    //free(path_bloques);
}

char* tomar_nombre_devolver_path(char* nombre){

    char* path_base = malloc(sizeof(path_base_dialfs));
    path_base = path_base_dialfs;
    
    if (path_base == NULL) {
        fprintf(stderr, "Error: path_base_dialfs no está inicializado.\n");
        return NULL;
    }

    char* archivo_a_devolver = malloc(strlen(path_base) + strlen(nombre) + 1 );
    memcpy(archivo_a_devolver, path_base, strlen(path_base));
    strcat(archivo_a_devolver, nombre);
    free(path_base);
    return archivo_a_devolver;
}

int redondear_up(int divisor, int dividendo){
    if(divisor % dividendo != 0){return (divisor/dividendo) + 1;}
    return divisor/dividendo;
}

void finalizar_entradasalida(){
    log_destroy(logger_entradasalida);
    config_destroy(config_entradasalida);
}

int leer_tipo_interfaz(t_config* config_entradasalida){
        char* tipo = config_get_string_value(config_entradasalida, "TIPO_INTERFAZ");
    if ( string_equals_ignore_case(tipo, "GENERICA") ) {
        return GENERICA;
    } else if ( string_equals_ignore_case(tipo, "STDIN") ) {
        return STDIN;
    } else if ( string_equals_ignore_case(tipo, "STDOUT") ) {
        return STDOUT;
    } else if ( string_equals_ignore_case(tipo, "DIALFS") ) {
        return DIAL_FS;
    } else {
        log_error(logger_entradasalida, "Tipo de interfaz no reconocida: %s", tipo);
        return (-1);
    }
}

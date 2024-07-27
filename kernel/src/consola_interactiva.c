#include "consola_interactiva.h"

bool flag_planificacion_detenido;
int diferencia_de_multiprogramacion;
//sem_t sem_planificador_LP_detenido;
//sem_t sem_planificador_CP_detenido;

char** comando_consola_desc[9] = {"EJECUTAR_SCRIPT", "INICIAR_PROCESO", "FINALIZAR_PROCESO", "INICIAR_PLANIFICACION", "DETENER_PLANIFICACION",
							      "PROCESO_ESTADO", "MULTIPROGRAMACION", "MENSAJE_A_MEMORIA1", "COMANDO_INVALIDO"};


void iniciar_consola_interactiva(){
    char* leido;
    imprimir_comandos_validos();
	leido = readline("> ");
    comando_consola comando;
	while(strcmp(leido, "\0") != 0){

        char** operacion = string_split(leido, " ");
        //printf("Operacion[0]: %s\nOperacion[1]: %s\n", operacion[0], operacion[1]);
        
        comando = validar_entrada(operacion[0]);
        // printf("Comando: %i\n", comando);
        // Operacion[0] = "INICIAR_PROCESO"
        // Operacion[1] = "path.txt"
        // Comando = 1;
        ejecutar_instruccion(operacion, comando); // (["INICIAR_PROCESO", "path.txt"], 1)
            //crear_proceso("as");


        free(leido);
		leido = readline("> ");
	}
	free(leido);
}

void imprimir_comandos_validos() {
    printf("\nComandos validos: \n");
    for (unsigned i = 0; i < 9; i++) {
        printf("%s ", comando_consola_desc[i]);
    }
    printf("; \n\n");
}

comando_consola validar_entrada(char* codigo){
    for(int i=0; i < 9; i++){
        if(!strcmp(codigo, comando_consola_desc[i])){
            return i;
        }
    }
    return (-1);
}



// (["INICIAR_PROCESO", "path.txt"], 1)
void ejecutar_instruccion(char** comando_desde_consola, comando_consola comando){
    //char** comando_desde_c = string_split(comando_desde_consola, " ");
    // printf("Comando: %i\nComando desde consola: %s\n", comando, comando_desde_consola[1]);

    t_buffer* buffer_a_enviar = crear_buffer();
    switch(comando){

        
        case EJECUTAR_SCRIPT:
            log_info(logger_kernel,"HOLA ENTRASTE A EJECUTAR SCRIPT\n");
            char* path = comando_desde_consola[1];
            int* contador = 0;
            char** comandos_de_script[100];
            abrir_archivo(comandos_de_script, path, contador);
            
            for(int i = 0; i<contador ; i++){
                ejecutar_instruccion(comandos_de_script[i], validar_entrada(comandos_de_script[i][0]));
            }
            break;
        

        case INICIAR_PROCESO:
            log_info(logger_kernel,"HOLA ENTRASTE A I.PROCESO\n");
            crear_proceso(comando_desde_consola[1]);
            break;
        

        case FINALIZAR_PROCESO:
            log_info(logger_kernel, "HOLA ENTRASTE A F.PROCESO");
            char* pid_char = comando_desde_consola[1];
            int pid = atoi(pid_char);
            if (RUNNING->pid == pid){
                interrumpir_cpu(RUNNING, ELIMINAR_PROCESO);
            } else {
                pasar_proceso_a_exit(pid, "INTERRUPTED BY USER");
            }
            break;
        

        case INICIAR_PLANIFICACION:
            log_info(logger_kernel, "HOLA ENTRASTE A I.PLANI");
            if(flag_planificacion_detenido){
                
                sem_post(&sem_planificador_LP_detenido);
                sem_post(&sem_planificador_CP_detenido);
                flag_planificacion_detenido = false;
            }
            break;
        

        case DETENER_PLANIFICACION:
            log_info(logger_kernel, "HOLA ENTRASTE A D.PLANI");
            if(!flag_planificacion_detenido){
                flag_planificacion_detenido = true;
                
            }
            break;
        

        case PROCESO_ESTADO:
            log_info(logger_kernel, "HOLA ENTRASTE A PROCESO ESTADO");
            break;
        

        case MULTIPROGRAMACION:
            log_info(logger_kernel, "HOLA ENTRASTE A MULTI");
            char* grado_char = comando_desde_consola[1];
            int nuevo_grado = atoi(grado_char);

            int viejo_grado = grado_multiprogramacion;
            diferencia_de_multiprogramacion =  viejo_grado - nuevo_grado;

            pthread_mutex_lock(&mutex_multiprogramacion);
            grado_multiprogramacion = nuevo_grado;
            pthread_mutex_unlock(&mutex_multiprogramacion);
            break;
        
        case MENSAJE_A_MEMORIA1:
            log_info(logger_kernel, "HOLA ENVIANDO MENSAJE");
            cargar_string_al_buffer(buffer_a_enviar, comando_desde_consola[1]);
            t_paquete* paquete = crear_paquete(MENSAJE_A_MEMORIA, buffer_a_enviar);
            enviar_paquete(paquete, fd_memoria);
            eliminar_paquete(paquete);
            break;

        default:
            log_error(logger_kernel, "Comando no reconocido");
            //exit(EXIT_FAILURE);
            break;

    }


}

void abrir_archivo(char** comandos_de_script[],char* path, int* j){

    FILE* archivo = fopen(path, "r+");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s\n", path);
        return 1;
    }
    int i = 0;
    
    while(!feof(archivo)){
        char* linea;
        fgets(linea, 100, archivo);
        char** comando = string_split(linea, " ");
        memcpy(comandos_de_script[i], comando, string_length(comando) + 1);
        i++;
    }
    j = i;
}


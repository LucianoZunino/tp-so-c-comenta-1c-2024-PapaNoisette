#include "consola_interactiva.h"

bool flag_planificacion_detenido;
int diferencia_de_multiprogramacion;
//sem_t sem_planificador_LP_detenido;
//sem_t sem_planificador_CP_detenido;

char** comando_consola_desc[9] = {"EJECUTAR_SCRIPT", "INICIAR_PROCESO", "FINALIZAR_PROCESO", "INICIAR_PLANIFICACION", "DETENER_PLANIFICACION",
							      "PROCESO_ESTADO", "MULTIPROGRAMACION", "MENSAJE_A_MEMORIA1", "COMANDO_INVALIDO"};


void iniciar_consola_interactiva(){
    char* leido;

	leido = readline("> ");
    comando_consola comando;
	while(strcmp(leido, "\0") != 0){

        char** operacion = string_split(leido, " ");
        
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

comando_consola validar_entrada(char* codigo){
    
    for(int i=0; i < 9; i++){
        if(strcmp(codigo, comando_consola_desc[i]) == 0){
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
            
            
            char* path = string_duplicate(comando_desde_consola[1]);
            //int* contador = 0;
            //char** comandos_de_script[100] = malloc(sizeof(char)*100);
            char** comandos_de_script = malloc(500);
            int contador = abrir_archivo(comandos_de_script, path);
            
            for(int i = 0; i<contador ; i++){
                char** instruccion = string_split(comandos_de_script[i], " ");
                ejecutar_instruccion(instruccion, validar_entrada(instruccion[0]));
            }
            free(comandos_de_script);

            break;
        case INICIAR_PROCESO:
            
            crear_proceso(comando_desde_consola[1]);

            break;
        case FINALIZAR_PROCESO:
            char* pid_char = comando_desde_consola[1];
            int pid = atoi(pid_char);
            //printf("\nPID RUNNING: %i --- PID INTERRUPT: %i\n", RUNNING->pid, pid);
            
            if(pasar_proceso_a_exit(pid, "INTERRUPTED BY USER") == NULL){
                interrumpir_cpu(RUNNING, ELIMINAR_PROCESO);
            }

            break;
        case INICIAR_PLANIFICACION:

            if(flag_planificacion_detenido){
                sem_post(&sem_planificador_LP_detenido);
                sem_post(&sem_planificador_CP_detenido);
                flag_planificacion_detenido = false;
            }

            break;
        case DETENER_PLANIFICACION:
        
            if(!flag_planificacion_detenido){
                flag_planificacion_detenido = true;
            }

            break;
        case PROCESO_ESTADO:
            for(int i = 0; i < 5; i++){ 
                log_info(logger_kernel, "Cola %s: ", estado_pcb_desc[i]);
                leer_pids_cola(i);
            }
            
            if(RUNNING != NULL){
                log_info(logger_kernel ,"Proceso en Execute: ");
                log_info(logger_kernel ,"   - PID: %i", RUNNING->pid);
            }
            
            break;
        case MULTIPROGRAMACION:
            char* grado_char = comando_desde_consola[1];
            int nuevo_grado = atoi(grado_char);

            int viejo_grado = grado_multiprogramacion;
            diferencia_de_multiprogramacion =  viejo_grado - nuevo_grado;

            pthread_mutex_lock(&mutex_multiprogramacion);
            grado_multiprogramacion = nuevo_grado;
            pthread_mutex_unlock(&mutex_multiprogramacion);

            if(viejo_grado < nuevo_grado){
                for(int i = 0; i < nuevo_grado - viejo_grado; i++){
                    sem_post(&sem_MULTIPROGRAMACION);
                }
            }
            break;
        case MENSAJE_A_MEMORIA1:
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

int abrir_archivo(char** comandos_de_script, char* path){


    FILE* archivo = fopen(path, "r+");

    if(archivo == NULL){
        printf("No se pudo abrir el archivo %s\n", path);
        return 1;
    }
    
 
    int i = 0;
    /*while(!feof(archivo)){
        char* linea = malloc(100 * sizeof(char));
        fgets(linea, 100, archivo);

        //printf("\nQue tiene comando: %s %s\n", comando[0] , comando[1]);
        
        printf("\n comando_de_script[%i] : %s \n", i, linea);
        char* comando_actual = string_duplicate(linea);
        memcpy(comandos_de_script[i], linea, string_length(linea));
        //strcpy(comandos_de_script[i], linea);
        
        printf("\n desp de memcpy \n");
        
        free(linea);
        i++;
    }*/

    while (!feof(archivo)) {
        char* linea = malloc(100 * sizeof(char));
        if (linea == NULL) {
            perror("Error al asignar memoria para linea");
            exit(EXIT_FAILURE);
        }

        fgets(linea, 100, archivo);
        if (ferror(archivo)) {
            perror("Error al leer el archivo");
            free(linea);
            break;
        }

        // Asignar memoria para almacenar la línea en comandos_de_script
        comandos_de_script[i] = malloc((strlen(linea) + 1) * sizeof(char));
        if (comandos_de_script[i] == NULL) {
            perror("Error al asignar memoria para comandos_de_script[i]");
            free(linea);
            exit(EXIT_FAILURE);
        }

        // Copiar la línea leída en comandos_de_script[i]
        strcpy(comandos_de_script[i], linea);

        

        i++;
        free(linea);
    }

    return i;
}

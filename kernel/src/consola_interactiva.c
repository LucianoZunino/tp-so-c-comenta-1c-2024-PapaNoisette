#include "consola_interactiva.h"

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

char* recibir_path(){
    return NULL;
}

// (["INICIAR_PROCESO", "path.txt"], 1)
void ejecutar_instruccion(char** comando_desde_consola, comando_consola comando){
    //char** comando_desde_c = string_split(comando_desde_consola, " ");
    // printf("Comando: %i\nComando desde consola: %s\n", comando, comando_desde_consola[1]);

    t_buffer* buffer_a_enviar = crear_buffer();
    switch(comando){

        
        case EJECUTAR_SCRIPT:
            log_info(logger_kernel,"HOLA ENTRASTE A EJECUTAR SCRIPT\n");
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
                pasar_proceso_a_exit(pid);
            }
            break;
        

        case INICIAR_PLANIFICACION:
            log_info(logger_kernel, "HOLA ENTRASTE A I.PLANI");
            break;
        

        case DETENER_PLANIFICACION:
            log_info(logger_kernel, "HOLA ENTRASTE A D.PLANI");
            break;
        

        case PROCESO_ESTADO:
            log_info(logger_kernel, "HOLA ENTRASTE A PROCESO ESTADO");
            break;
        

        case MULTIPROGRAMACION:
            log_info(logger_kernel, "HOLA ENTRASTE A MULTI");
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
             exit(EXIT_FAILURE);
         

    }


}
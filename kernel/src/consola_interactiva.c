#include "consola_interactiva.h"

void iniciar_consola_interactiva(){
    char* leido;
	leido = readline("> ");
    bool comando_valido;
	log_info(logger_kernel, "FUERA DEL WHILE");
	while(strcmp(leido, "\0") != 0){
        log_info(logger_kernel, "DENTRO DEL WHILE");
        comando_valido = validacion_de_instruccion_de_consola(leido);
        
        if(comando_valido){
            log_info(logger_kernel, "POR EJECUTAR INSTRUCCION");
            ejecutar_instruccion(leido);

            free(leido);
		    leido = readline("> ");
        }
        else{
            log_error(logger_kernel, "Comando no válido");
            free(leido);
            leido = readline("> ");
            continue; // Vuelve al inicio del while
        }
	}
	free(leido);
}
void validacion_correcta(bool* comando_valido){
    *comando_valido = true;
    log_info(logger_kernel, "VALIDACION CORRECTA");
}

bool validacion_de_instruccion_de_consola(char* leido){
 
    bool comando_valido = false;
    char** comando_desde_consola = string_split(leido, " ");
    log_info(logger_kernel, "DENTRO DEL VALIDACION");
    /*if(strcmp(comando_desde_consola[0], "MENSAJE_A_MEMORIA") == 0){
        comando_valido = true;
        log_info(logger_kernel, "VALIDACION CORRECTA");
    }
    else{
        log_error(logger_kernel, "Comando no reconocido");
    }*/
     if (strcmp(comando_desde_consola[0], "EJECUTAR_SCRIPT") == 0) {
       validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "INICIAR_PROCESO") == 0) {
        validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "FINALIZAR_PROCESO") == 0) {
        validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "INICIAR_PLANIFICACION") == 0) {
        validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "DETENER_PLANIFICACION") == 0) {
        validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "PROCESO_ESTADO") == 0) {
        validacion_correcta(&comando_valido);

    } else if (strcmp(comando_desde_consola[0], "MULTIPROGRAMACION") == 0) {
        validacion_correcta(&comando_valido);
        
    } else {
        log_error(logger_kernel, "Comando no reconocido");
        exit(EXIT_FAILURE);
    }

    string_array_destroy(comando_desde_consola);

    return comando_valido;
}

void ejecutar_instruccion(char* leido){
    char** comando_desde_consola = string_split(leido, " ");
 //   t_buffer* buffer_a_enviar = crear_buffer();

  /*  if(strcmp(comando_desde_consola[0], "MENSAJE_A_MEMORIA") == 0){
        log_info(logger_kernel, "DENTRO DEL IF DE EJECUTAR INSTRUCCION");
        cargar_string_al_buffer(buffer_a_enviar, comando_desde_consola[1]);
        t_paquete* paquete = crear_paquete(MENSAJE_A_MEMORIA, buffer_a_enviar);
        enviar_paquete(paquete, fd_memoria);
        eliminar_paquete(paquete);
    }
    else{
        log_error(logger_kernel, "Comando no reconocido");
        exit(EXIT_FAILURE);
    }*/
    log_info(logger_kernel, "FUERA DEL IF DE EJECUTAR INSTRUCCION");
    if (strcmp(comando_desde_consola[0], "EJECUTAR_SCRIPT") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A EJECUTAR SCRIPT\n");

    } else if (strcmp(comando_desde_consola[0], "INICIAR_PROCESO") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A I.PROCESO\n");

    } else if (strcmp(comando_desde_consola[0], "FINALIZAR_PROCESO") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A F.PROCESO\n");

    } else if (strcmp(comando_desde_consola[0], "INICIAR_PLANIFICACION") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A I.PLANI\n");

    } else if (strcmp(comando_desde_consola[0], "DETENER_PLANIFICACION") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A D.PLANI\n");

    } else if (strcmp(comando_desde_consola[0], "PROCESO_ESTADO") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A PROCESO ESTADO\n");

    } else if (strcmp(comando_desde_consola[0], "MULTIPROGRAMACION") == 0) {
        log_info(logger_kernel,"HOLA ENTRASTE A MULTI\n");

    } else {
        log_error(logger_kernel, "Comando no reconocido");
        exit(EXIT_FAILURE);
    }


    string_array_destroy(comando_desde_consola);
}
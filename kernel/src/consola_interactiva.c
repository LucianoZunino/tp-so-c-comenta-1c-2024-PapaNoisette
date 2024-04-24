#include "consola_interactiva.h"

void iniciar_consola_interactiva(){
    char* leido;
	leido = readline("> ");
    bool comando_valido;
	
	while(strcmp(leido, "\0") != 0){
        comando_valido = validacion_de_instruccion_de_consola(leido);
        if(comando_valido){
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

bool validacion_de_instruccion_de_consola(char* leido){
 
    bool comando_valido = false;
    char** comando_desde_consola = string_split(leido, " ");

    if(strcmp(comando_desde_consola[0], "MENSAJE_A_MEMORIA") == 0){
        comando_valido = true;
    }
    else{
        log_error(logger_kernel, "Comando no reconocido");
    }

    string_array_destroy(comando_desde_consola);

    return comando_valido;
}

void ejecutar_instruccion(char* leido){
    char** comando_desde_consola = string_split(leido, " ");
    t_buffer* buffer_a_enviar = crear_buffer();

    if(strcmp(comando_desde_consola[0], "MENSAJE_A_MEMORIA") == 0){
        cargar_string_al_buffer(buffer_a_enviar, comando_desde_consola[1]);
        t_paquete* paquete = crear_paquete(MENSAJE_A_MEMORIA, buffer_a_enviar);
        enviar_paquete(paquete, fd_memoria);
        eliminar_paquete(paquete);
    }
    else{
        log_error(logger_kernel, "Comando no reconocido");
        exit(EXIT_FAILURE);
    }

    string_array_destroy(comando_desde_consola);
}
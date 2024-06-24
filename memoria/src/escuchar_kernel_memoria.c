#include "escuchar_kernel_memoria.h"

void escuchar_mensajes_kernel_memoria(){
    bool desconexion_kernel_memoria = 0;
	t_buffer* buffer;

	while(!desconexion_kernel_memoria){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case MENSAJE_A_MEMORIA:
				buffer = recibir_buffer_completo(fd_kernel);
				//obtener_mensaje(buffer);
				break;
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_memoria, fd_kernel, cod_op);
				break;
			case MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS:
                // ver de crear hilo¿?
                buffer = recibir_buffer_completo(fd_kernel);
                printf("PASO EL RECIBIR_BUFFER\n");
                t_pcb* pcb = deserializar_pcb(buffer);
                printf("Deserializar PCB, PID: %i\n", pcb->pid);
                char* path = extraer_string_del_buffer(buffer); // Obtiene el path del archivo pseudocodigo
                printf("Extraer path: %s\n", path);
                guardar_instrucciones_en_memoria(pcb, path); //Guar
                printf("Instrucciones guardadas en memoria\n");

                int numero_paginas_inicial = 0; // Número de páginas inicial
                TablaDePaginasPorProceso *tabla_de_paginas_del_proceso = memoria_crear_proceso(pcb->pid, numero_paginas_inicial);
                list_add(lista_de_tablas_de_paginas_por_proceso, tabla_de_paginas_del_proceso);

                /*if(tabla_de_paginas_del_proceso != NULL){
                    printf("Proceso con PID %d tiene una tabla de páginas inicializada con %d páginas\n", tabla_de_paginas_del_proceso->pid, tabla_de_paginas_del_proceso->tabla_paginas->numero_paginas);
                    // Aquí puedes trabajar con el proceso y su tabla de páginas
                    // ESTO ES PARA PROBAAAAAAAAAAAAAAAAAAAAAR
                    // Liberar memoria al finalizar
                    free(tabla_de_paginas_del_proceso->tabla_paginas->pagina);
                    free(tabla_de_paginas_del_proceso->tabla_paginas);
                    free(tabla_de_paginas_del_proceso);
                }*/

                printf("Antes de enviar_ok\n");
                enviar_ok(KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS, fd_kernel);
                printf("Después de enviar_ok\n");
                destruir_buffer(buffer);
                printf("Después de destruir el buffer\n");
				break;


            case LIBERAR_PROCESO_EN_MEMORIA:
                int pid = recibir_int(fd_kernel);
                finalizar_proceso(pid);
                break;
			case -1:
				log_error(logger_memoria, "El Kernel se desconecto de Memoria. Terminando servidor.");
				desconexion_kernel_memoria = 1;
				break;
			default:
				log_warning(logger_memoria, "Operacion desconocida de Kernel-Memoria.");
				break;
			}
	}
}

int recibir_path_kernel(int socket){
    op_code codigo_de_operacion =  recibir_codigo_operacion(socket);

    if (codigo_de_operacion == MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS){

    }
}

void guardar_instrucciones_en_memoria(t_pcb* pcb,char* path){
    
    t_miniPcb* instrucciones = malloc(sizeof(t_miniPcb));

    FILE* archivo = fopen(path, "r");
    // La ruta desde memoria sería: "../kernel/path.txt"
    if(archivo == NULL){
        printf("Error: No se pudo abrir el archivo %s\n", path);
        return;
    }
    printf("Archivo != NULL\n");
    instrucciones->pid = malloc(sizeof(int));
    instrucciones->pid = pcb->pid;
    instrucciones->lista_de_instrucciones = list_create(); // Crea una lista ej => ["MOVE AX BX", "ADD DX CX"]
    
    char* linea[100]; 
    printf("Antes del while\n");
    while(fgets(linea, sizeof(linea), archivo) != NULL){
        // Eliminar el salto de línea al final de la línea leída
        linea[strcspn(linea, "\n")] = '\0';
        // Crear una copia de la línea leída para almacenarla en la lista
        char* linea_copia = strdup(linea);
        printf("Procesada linea \" %s \" \n", linea_copia);
        char* linea_a_agregar = malloc(sizeof(linea_copia)); // HACER FREE AL SACAR EL PROCESO DE MEMORIA
        list_add(instrucciones->lista_de_instrucciones, linea_a_agregar); // Agrega las intrucciones a la lista
    }
    printf("Después del while\n");
    fclose(archivo);
    printf("Archivo cerrado\n");
    printf("Size de lista_de_miniPcb: %i\n", list_size(lista_de_miniPcb));
    // PONER HILO MUTEX PARA LISTA_dE_MINIPCB
    list_add(lista_de_miniPcb, instrucciones ); 
    printf("Fin de guardar_instrucciones_en_memoria\n");
}


/*
int recv_fetch_instruccion(int fd_modulo, char** path, int** pc) {
    t_list* paquete = recibir_paquete(fd_modulo); // Preguntar funcion q recibe paquete

	*path = (char*) list_get(paquete, 0); // Obtener el path del paquete
    *pc = (int*) list_get(paquete, 1);

    list_destroy(paquete);
    return 0; 
}

void obtener_mensaje(t_buffer* buffer){
	char* mensaje = extraer_string_del_buffer(buffer);
	log_info(logger_memoria, "Mensaje recibido: %s", mensaje);
	free(mensaje);
}

char *armar_path_instruccion(char *path_consola) {
    char *path_completo = string_new();
    string_append(&path_completo, path_instrucciones);
    string_append(&path_completo, "/");
    string_append(&path_completo, path_consola);
    return path_completo;
}

void leer_instruccion_por_pc_y_enviar(char *path_consola, int pc, int fd) {
    char *path_completa_instruccion = armar_path_instruccion(path_consola);

    FILE *archivo = fopen(path_completa_instruccion, "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo de instrucciones");
        free(path_completa_instruccion);
        return;
    }

    char instruccion_leida[256];
    int current_pc = 0;

    while (fgets(instruccion_leida, sizeof(instruccion_leida), archivo) != NULL) {
        if (current_pc == pc) {
            printf("Instrucción %d: %s", pc, instruccion_leida);
            Instruccion instruccion = armar_estructura_instruccion(instruccion_leida);

            send_instruccion(fd, instruccion);

            // Liberar memoria asignada para la estructura Instruccion

            free(instruccion.opcode);
            free(instruccion.operando1);
            free(instruccion.operando2);
            
            break;
        }
        current_pc++;
    }
    fclose(archivo);
    free(path_completa_instruccion);
}

/*t_instruccion armar_estructura_instruccion(char* instruccion_leida){
    char **palabras = string_split(instruccion_leida, " ");
    
    t_instruccion instruccion;

    if (palabras[0] != NULL) {
        instruccion.opcode = malloc(sizeof(char) * (strlen(palabras[0]) + 1));
        strcpy(instruccion.opcode, palabras[0]);

        if (palabras[1] != NULL) {
            instruccion.operando1 = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
            strcpy(instruccion.operando1, palabras[1]);

            if (instruccion.operando1[strlen(instruccion.operando1) - 1] == '\n') {
                instruccion.operando1[strlen(instruccion.operando1) - 1] = '\0';
            }

            if (palabras[2] != NULL) {
                instruccion.operando2 = malloc(sizeof(char) * (strlen(palabras[2]) + 1));
                strcpy(instruccion.operando2, palabras[2]);

                if (instruccion.operando2[strlen(instruccion.operando2) - 1] == '\n') {
                    instruccion.operando2[strlen(instruccion.operando2) - 1] = '\0';
                }
            } else {
                instruccion.operando2 = malloc(sizeof(char));
                instruccion.operando2[0] = '\0'; // Vaciar el operando2 si no hay tercer palabra
            }
        } else {
            instruccion.operando1 = malloc(sizeof(char));
            instruccion.operando2 = malloc(sizeof(char));
            instruccion.operando1[0] = '\0'; // Vaciar el operando1 si no hay segunda palabra
            instruccion.operando2[0] = '\0'; // Vaciar el operando2
        }
    } else {
        perror("Error al cargar la instrucción");

        // Liberar memoria en caso de error
        free(instruccion.opcode);
        free(instruccion.operando1);
        free(instruccion.operando2);
        instruccion.opcode = NULL;
        instruccion.operando1 = NULL;
        instruccion.operando2 = NULL;
    }

    // Liberar memoria asignada a palabras
    int i = 0;
    while (palabras[i] != NULL) {
        free(palabras[i]);
        i++;
    }
    free(palabras);

    return instruccion;
}*/
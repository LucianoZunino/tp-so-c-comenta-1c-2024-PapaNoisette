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
			case  HANDSHAKE_KERNEL:
				aceptar_handshake(logger_memoria, fd_kernel, cod_op);
				break;
			case MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS:
            	log_info(logger_memoria, "MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS");

                // ver de crear hilo¿?
                buffer = recibir_buffer_completo(fd_kernel);
                printf("PASO EL RECIBIR_BUFFER\n");
                t_pcb* pcb = deserializar_pcb(buffer);
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


void guardar_instrucciones_en_memoria(t_pcb* pcb,char* path){
    
    t_miniPcb* miniPcb = malloc(sizeof(t_miniPcb));
    //char *path_final=strcat(strcat(path_instrucciones,"/"),path);
    char *path_final="/home/utnso/scripts-pruebas/instrucciones.txt";
    FILE* archivo = fopen(path_final, "r");

    if(archivo == NULL){
        log_error(logger_memoria, "No se pudo abrir el archivo %s",path_final);

        return;
    }
    miniPcb->pid = malloc(sizeof(int));
    miniPcb->pid = pcb->pid;
    miniPcb->lista_de_instrucciones = list_create(); // Crea una lista ej => ["MOVE AX BX", "ADD DX CX"]

    char linea[100]; 
    
    while(fgets(linea, sizeof(linea), archivo) != NULL){
        // Eliminar el salto de línea al final de la línea leída
        linea[strcspn(linea, "\n")] = '\0';
        // Crear una copia de la línea leída para almacenarla en la lista
        char* linea_copia = strdup(linea);
        printf("Procesada linea \" %s \" \n", linea_copia);
        char* linea_a_agregar = malloc(sizeof(linea_copia)); // HACER FREE AL SACAR EL PROCESO DE MEMORIA
        list_add(miniPcb->lista_de_instrucciones, linea_copia); // Agrega las intrucciones a la lista
    }
    fclose(archivo);
    printf("Archivo cerrado\n");
    printf("Size de lista_de_miniPcb: %i\n", list_size(lista_de_miniPcb));
    // PONER HILO MUTEX PARA LISTA_dE_MINIPCB
    list_add(lista_de_miniPcb, miniPcb ); 

}


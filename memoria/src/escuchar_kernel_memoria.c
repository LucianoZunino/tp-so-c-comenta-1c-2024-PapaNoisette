#include "escuchar_kernel_memoria.h"

void escuchar_mensajes_kernel_memoria(){
    bool desconexion_kernel_memoria = 0;
	t_buffer* buffer;
    int pid;
    
	while(!desconexion_kernel_memoria){
        int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
        switch(cod_op){
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_memoria, fd_kernel, cod_op);
                
				break;
			case MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS:
            	log_info(logger_memoria, "MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS");
                buffer = recibir_buffer_completo(fd_kernel);
                pid = extraer_int_del_buffer(buffer); // Obtiene el path del archivo pseudocodigo
                char* path = extraer_string_del_buffer(buffer); // Obtiene el path del archivo pseudocodigo
                printf("Extraer path: %s\n", path);
                guardar_instrucciones_en_memoria(pid, path); //Guar
                printf("Instrucciones guardadas en memoria\n");

                memoria_crear_proceso(pid);

                enviar_ok(KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS, fd_kernel);
                destruir_buffer(buffer);

				break;
            case LIBERAR_PROCESO_EN_MEMORIA:
                buffer = recibir_buffer_completo(fd_kernel);
                pid = extraer_int_del_buffer(buffer);
                finalizar_proceso(pid);
                destruir_buffer(buffer);

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
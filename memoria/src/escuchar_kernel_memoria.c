#include "escuchar_kernel_memoria.h"

void escuchar_mensajes_kernel_memoria(){
    bool desconexion_kernel_memoria = 0;
	t_buffer* buffer;
	while(!desconexion_kernel_memoria){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case MENSAJE_A_MEMORIA:
				buffer = recibir_buffer_completo(fd_kernel);
				obtener_mensaje(buffer);
				break;
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_memoria, fd_kernel, cod_op);
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

void obtener_mensaje(t_buffer* buffer){
	char* mensaje = extraer_string_del_buffer(buffer);
	log_info(logger_memoria, "Mensaje recibido: %s", mensaje);
	free(mensaje);
}
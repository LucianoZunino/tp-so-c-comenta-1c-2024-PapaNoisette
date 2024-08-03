#include "escuchar_memoria_kernel.h"

void escuchar_mensajes_memoria_kernel(){
	bool desconexion_memoria_kernel = 0;
	while(!desconexion_memoria_kernel){
		int cod_op = recibir_operacion(fd_memoria); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS:
				sem_post(&sem_estructuras_inicializadas);
				break;
			case -1:
				log_error(logger_kernel, "La Memoria se desconecto de Kernel. Terminando servidor.");
				desconexion_memoria_kernel = 1;
				break;
			default:
				//log_warning(logger_kernel, "Operacion desconocida de Memoria-Kernel.");
				break;
			}
	}
}
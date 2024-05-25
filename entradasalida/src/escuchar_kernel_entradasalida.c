#include "escuchar_kernel_entradasalida.h"

void escuchar_mensajes_kernel_entradasalida(){
    bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
			case IO_GEN_SLEEP_FS:
				t_buffer * buffer;
				buffer=recibir_buffer_completo(fd_kernel);
				char * interfaz = extraer_string_del_buffer(buffer);
                int tiempo_retardo = extraer_int_del_buffer(buffer);
				if (interfaz==tipo_interfaz){ //si el sleep fue a este recurso
					sleep(tiempo_retardo);
				}
				else {
				log_trace(logger_entradasalida, "Se recibio un io_gen_sleep pero para otra interfaz.");

				}



			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_entradasalida, fd_kernel, cod_op);
				break;

			break;
			case -1:
				log_error(logger_entradasalida, "El Kernel se desconecto de Entradasalida. Terminando servidor.");
				desconexion_kernel_entradasalida = 1;
				break;
			default:
				log_warning(logger_entradasalida, "Operacion desconocida de Kernel-Entradasalida.");
				break;
			}
	}
}
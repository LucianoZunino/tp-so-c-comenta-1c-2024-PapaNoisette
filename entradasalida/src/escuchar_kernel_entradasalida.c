#include "escuchar_kernel_entradasalida.h"

t_dictionary* interfaces;

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
				/*
				char * interfaz = extraer_string_del_buffer(buffer);
                int tiempo_retardo = extraer_int_del_buffer(buffer);
				if (interfaz==tipo_interfaz){ //si el sleep fue a este recurso
					sleep(tiempo_retardo);
				}
				else {
				log_trace(logger_entradasalida, "Se recibio un io_gen_sleep pero para otra interfaz.");

				}*/
				char* nombre_interfaz = extraer_string_del_buffer(buffer);
				char* path_config_interfaz = extraer_string_del_buffer(buffer);
				crear_interfaz(nombre_interfaz, path_config_interfaz);

				break;
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_entradasalida, fd_kernel, cod_op);
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

void crear_interfaz(char* nombre_interfaz, char* path_config){
	t_config* config = iniciar_config(path_config);
	char* tipo_de_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");

	if (strcmp(tipo_de_interfaz, "GENERICA")){
		// crear interfaz generica
		t_queue* cola = queue_create();
		sem_t* semaforo = sem_init(semaforo, 1, 0);

		t_interfaz nueva_interfaz;
		nueva_interfaz.config = config;

		pthread_t* hilo = malloc(sizeof(pthread_t*));
		nueva_interfaz.hilo = hilo;
		nueva_interfaz.cola_espera = cola;
		nueva_interfaz.semaforo = semaforo;

		pthread_create(hilo, NULL, iniciar_interfaz_generica(&nueva_interfaz), NULL);
		pthread_detach(hilo);



		dictionary_put(interfaces, nombre_interfaz, &nueva_interfaz);
		sem_post(semaforo);
	}
	else if(strcmp(tipo_de_interfaz, "STDIN")){
		// TODO crear interfaz STDIN
	}
	else if(strcmp(tipo_de_interfaz, "STDOUT")){
		// TODO crear interfaz STDOUT
	}
	else if(strcmp(tipo_de_interfaz, "DIAL_FS")){
		// TODO crear interfaz DIAL_FS
	}
	else {
		log_error(logger_entradasalida, "El tipo de interfaz %s no existe", tipo_de_interfaz);
	}
}
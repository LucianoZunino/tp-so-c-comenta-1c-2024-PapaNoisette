#include "escuchar_kernel_entradasalida.h"
#include <readline/readline.h>

t_dictionary* interfaces;


void escuchar_instrucciones_generica(){
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			case IO_GEN_SLEEP_FS:
				t_buffer* buffer;
				buffer = recibir_buffer_completo(fd_kernel);
				
				int pid = extraer_int_del_buffer(buffer);
				int unidades = extraer_int_del_buffer(buffer);

				usleep(unidades * tiempo_unidad_trabajo);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				t_buffer* buffer_kernel;
				buffer_kernel = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer_kernel);
				free(buffer_kernel);

				buffer_kernel = crear_buffer();
				cargar_int_al_buffer(buffer_kernel, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer_kernel);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}


void escuchar_instrucciones_stdin(){
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer;
			int pid;
			case IO_STDIN_READ_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				printf("\nIngrese hasta %i caracteres\n", reg_tamanio);
				char* input = readline("> ");
				char* mensaje;
				memcpy(mensaje, input, reg_tamanio);

				solicitar_almacen_memoria(reg_direccion, mensaje);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_stdout(){
	bool desconexion_kernel_entradasalida = 0;
	while(!desconexion_kernel_entradasalida){
		int cod_op = recibir_operacion(fd_kernel); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			t_buffer* buffer;
			int pid;
			case IO_STDOUT_WRITE_FS:
				buffer = recibir_buffer_completo(fd_kernel);
				
				pid = extraer_int_del_buffer(buffer);
				int reg_direccion = extraer_int_del_buffer(buffer);
				int reg_tamanio = extraer_int_del_buffer(buffer);

				solicitar_lectura_memoria(reg_direccion, reg_tamanio);

				sem_wait(&sem_stdout);

				notificar_fin(fd_kernel, pid);

				destruir_buffer(buffer);
				break;
			default: // La instruccion es incorrecta
				buffer = recibir_buffer_completo(fd_kernel);
				int process_id = extraer_int_del_buffer(buffer);
				free(buffer);

				buffer = crear_buffer();
				cargar_int_al_buffer(buffer, process_id);

				t_paquete* paquete = crear_paquete(ERROR_IO, buffer);
				enviar_paquete(paquete, fd_kernel);

				eliminar_paquete(paquete);
				destruir_buffer(buffer);
				break;
		}
	}
}

void escuchar_instrucciones_dialfs(){

}



void notificar_fin(int fd_kernel, int pid){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, pid);

	t_paquete* paquete = crear_paquete(FIN_IO, buffer);
	enviar_paquete(paquete, fd_kernel);

	eliminar_paquete(paquete);
	free(buffer);
}

void solicitar_lectura_memoria(int direccion, int tamanio){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_int_al_buffer(buffer, tamanio);

	t_paquete* paquete = crear_paquete(IO_STDOUT_WRITE_FS, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	free(buffer);
}

void solicitar_almacen_memoria(int direccion, char* mensaje){
	t_buffer* buffer = crear_buffer();
	cargar_int_al_buffer(buffer, direccion);
	cargar_string_al_buffer(buffer, mensaje);

	t_paquete* paquete = crear_paquete(IO_STDIN_READ_FS, buffer);

	enviar_paquete(paquete, fd_memoria);

	eliminar_paquete(paquete);
	free(buffer);
}
/*
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
				
				//char * interfaz = extraer_string_del_buffer(buffer);
                /int tiempo_retardo = extraer_int_del_buffer(buffer);
				//if (interfaz==tipo_interfaz){ //si el sleep fue a este recurso
				//	sleep(tiempo_retardo);
				//}
				//else {
				//log_trace(logger_entradasalida, "Se recibio un io_gen_sleep pero para otra interfaz.");

				}
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
*/
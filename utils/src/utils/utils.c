#include "utils.h"

t_config* iniciar_config(char* ruta_config){
	t_config* nuevo_config = config_create(ruta_config);
	if (nuevo_config == NULL){
		printf("Error al crear el config %s.\n", ruta_config);
		exit(1);
	}
	return nuevo_config;
}

t_log* iniciar_logger(char* ruta_logger, char* nombre_logger){
	t_log* nuevo_logger = log_create(ruta_logger, nombre_logger, true, LOG_LEVEL_INFO); // el LEVEL tendría que ser un parámetro si vamos a usar distintos niveles
	if (nuevo_logger == NULL){
		printf("Error al crear el logger %s.\n", ruta_logger);
		exit(1);
	}

	return nuevo_logger;
}
// FUNCIONES DE CONEXION DE CLIENTE
int crear_conexion(char *ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                    server_info->ai_socktype,
                    server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	//continuando sobre cuando creamos el socket del cliente

	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);
	return socket_cliente;
}

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}

// FUNCIONES DE CONEXION DE SERVIDOR
int iniciar_servidor(char* puerto, t_log* logger, char* mensaje_servidor){

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo); 

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen); // falta chequear si esto da error
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN); // falta chequear si esto da error

	freeaddrinfo(servinfo);
	log_info(logger, mensaje_servidor);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log* logger, char* cliente){

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto el cliente:", cliente);
	printf("(PRINTF) Se conecto el cliente: %s\n", cliente);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente){
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

//para los arrays de kernel
int largo_array(char**array) { 
	int largo = 0;
	while (array[largo] != NULL) {
		largo++;
	}
	return largo;
}


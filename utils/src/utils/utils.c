#include "utils.h"

char* op_code_desc[] = {"HANDSHAKE_OK", "MENSAJE_A_MEMORIA", "HANDSHAKE_KERNEL", "HANDSHAKE_ENTRADASALIDA", "HANDSHAKE_MEMORIA", "HANDSHAKE_CPU"};


t_config* iniciar_config(char* ruta_config){
	t_config* nuevo_config = config_create(ruta_config);
	if(nuevo_config == NULL)
	{
		printf("Error al cargar el config %s.\n", ruta_config);
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}


t_log *iniciar_logger(char *ruta_logger, char *nombre_logger)
{
	t_log *nuevo_logger = log_create(ruta_logger, nombre_logger, true, LOG_LEVEL_INFO); // el LEVEL tendría que ser un parámetro si vamos a usar distintos niveles
	if (nuevo_logger == NULL)
	{
		printf("Error al crear el logger %s.\n", ruta_logger);
		exit(EXIT_FAILURE);
	}
	return nuevo_logger;
}

// ----- FUNCION DE CONEXION DE CLIENTE -> SERVIDOR -----

int crear_conexion(char *ip, char *puerto, t_log *logger)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Crea el socket
	int socket_cliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	// continuando sobre cuando creamos elp socket del cliente

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0)
	{
		log_error(logger, "Error conectando al Servidor,apagado o invalido");
		socket_cliente = -1;
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

// ----- FUNCIONES DE CONEXION DE SERVIDOR -> CLIENTE -----

int iniciar_servidor(char *puerto, t_log *logger, char *mensaje_servidor)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_servidor == -1)
	{
		log_error(logger, "Error iniciando Servidor, fallo al crear el socket");
	}
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO//eliminar antes de la entrega, es para reutilizar los sockets y que no queden colgados al hacer pruebas
	int activado = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	// Asociamos el socket a un puerto
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
	{
		close(socket_servidor);
		log_error(logger, "Error iniciando Servidor, fallo al bindear el socket");
		return -1;
	}
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(logger, "%s", mensaje_servidor);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log *logger, char *cliente)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto el cliente: %s\n", cliente);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

t_pcb* recibir_pcb(int socket_cliente)
{
	t_pcb* pcb;
	if (recv(socket_cliente, &pcb, sizeof(t_pcb*), MSG_WAITALL) > 0)
		return pcb;
	else
	{
		close(socket_cliente);
		return -1;
	}
}


/*void crear_buffer(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}*/

t_buffer *crear_buffer()
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	buffer->size = 0;
	buffer->stream = NULL;
	return buffer;
}

void destruir_buffer(t_buffer *buffer)
{
	if (buffer->stream != NULL)
	{
		free(buffer->stream);
	}
	free(buffer);
}

void cargar_datos_al_buffer(t_buffer *buffer, void *datos, int size_datos)
{
	if (buffer->size == 0)
	{
		buffer->stream = malloc(sizeof(int) + size_datos);
		memcpy(buffer->stream, &size_datos, sizeof(int));
		memcpy(buffer->stream + sizeof(int), datos, size_datos); // Desplaza '+ sizeof(int)' para que no pise lo que ya esta
	}
	else
	{
		buffer->stream = realloc(buffer->stream, buffer->size + sizeof(int) + size_datos);
		memcpy(buffer->stream + buffer->size, &size_datos, sizeof(int));
		memcpy(buffer->stream + buffer->size + sizeof(int), datos, size_datos);
	}
	buffer->size += sizeof(int);
	buffer->size += size_datos;
}

void cargar_int_al_buffer(t_buffer *buffer, int valor_int)
{
	cargar_datos_al_buffer(buffer, &valor_int, sizeof(int));
}

void cargar_string_al_buffer(t_buffer *buffer, char *valor_string)
{
	cargar_datos_al_buffer(buffer, valor_string, strlen(valor_string) + 1); // +1 por el /0
}

/*void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}*/

t_buffer *recibir_buffer_completo(int socket_cliente)
{ // Recibe todo lo enviado despues del OP_CODE ([cod_op][size][void*......])
	t_buffer *buffer = malloc(sizeof(t_buffer));

	if (recv(socket_cliente, &(buffer->size), sizeof(int), MSG_WAITALL) > 0)
	{ // Guardo el [size]
		buffer->stream = malloc(buffer->size);
		if (recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL) > 0)
		{ // Guardo el [void*.......]
			return buffer;
		}
		else
		{
			printf("Error al recibir el buffer");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("Error el recibir el tamaño del buffer");
		exit(EXIT_FAILURE);
	}
	return buffer;
}

void *extraer_datos_del_buffer(t_buffer *buffer)
{
	if (buffer->size == 0)
	{
		printf("El buffer está vacio");
		exit(EXIT_FAILURE);
	}
	int size_datos;
	memcpy(&size_datos, buffer->stream, sizeof(int));
	void *datos = malloc(size_datos);
	memcpy(datos, buffer->stream + sizeof(int), size_datos);

	int nuevo_size = buffer->size - sizeof(int) - size_datos;

	if (nuevo_size == 0)
	{ // Se extrajeron todos los datos
		buffer->size = 0;
		free(buffer->stream);
		buffer->stream = NULL;
		return datos;
	}

	void *nuevo_stream = malloc(nuevo_size);
	memcpy(nuevo_stream, buffer->stream + sizeof(int) + size_datos, nuevo_size);
	free(buffer->stream);
	buffer->size = nuevo_size;
	buffer->stream = nuevo_stream;

	return datos;
}

int extraer_int_del_buffer(t_buffer *buffer)
{
	int *valor_int = extraer_datos_del_buffer(buffer);
	int valor_int_retornado = *valor_int;
	free(valor_int); // Libera la memoria que reservo extraer_datos_del_buffer
	return valor_int_retornado;
}

char *extraer_string_del_buffer(t_buffer *buffer)
{
	char *valor_string = extraer_datos_del_buffer(buffer);
	return valor_string;
}

t_paquete *crear_paquete(op_code code_op, t_buffer *buffer)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = code_op;
	paquete->buffer = buffer;
	return paquete;
}

void eliminar_paquete(t_paquete *paquete)
{
	destruir_buffer(paquete->buffer);
	free(paquete);
}

void *serializar_paquete(t_paquete *paquete)
{
	int size_datos = paquete->buffer->size + 2 * sizeof(int);
	void *magic = malloc(size_datos);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializar_paquete(paquete);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

// Serializacion de las instrucciones que ejecuta cpu//despues veo si este codigo me lo llevo a al modulo correspondiente

void *serializar_instruccion(t_instruccion *instruccion)
{
	uint32_t tam_buffer_instruccion = sizeof(t_instruccion);
	//printf("sizeoft_instruccion %ld \n", sizeof(t_instruccion));

	void *buffer_instruccion = malloc(tam_buffer_instruccion);
	memcpy(buffer_instruccion, instruccion->instruccion, sizeof(char[20]));
	memcpy(buffer_instruccion + sizeof(char[20]), instruccion->arg1, sizeof(char[20]));
	memcpy(buffer_instruccion + sizeof(char[20]) * 2, instruccion->arg2, sizeof(char[20]));
	memcpy(buffer_instruccion + sizeof(char[20]) * 3, instruccion->arg3, sizeof(char[20]));

	return buffer_instruccion;
}

t_instruccion *deserializar_instruccion(void *stream)
{
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	memcpy(&instruccion->instruccion, stream, sizeof(char[20]));
	memcpy(&instruccion->arg1, stream + sizeof(char[20]), sizeof(char[20]));
	memcpy(&instruccion->arg2, stream + sizeof(char[20]) * 2, sizeof(char[20]));
	memcpy(&instruccion->arg3, stream + sizeof(char[20]) * 3, sizeof(char[20]));
	return instruccion;
}

// Para leer el script de instrucciones y lo deja enlistado para que memoria los envie de a uno
t_list *parsear_archivo_instrucciones(char *path_archivo, t_log *logger)// va a memoria
{
	FILE *archivo_instrucciones = fopen(path_archivo, "r");

	if (archivo_instrucciones == NULL)
	{
		log_error(logger, "Error al abrir el archivo de instrucciones\n");
		exit(-1);
	}

	t_list *instrucciones = list_create();
	char *linea = malloc(sizeof(t_instruccion));

	t_instruccion *instruccion;
	while (fgets(linea, 80, archivo_instrucciones) !=NULL)
	{

		printf("linea %s\n",linea);


		instruccion = malloc(sizeof(t_instruccion));
		instruccion->instruccion = malloc(20);
		instruccion->arg1 = malloc(20);
		instruccion->arg2 = malloc(20);
		instruccion->arg3 = malloc(20);

		if (sscanf(linea, "%s %s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3) == 4)
		{
			// se cargaron los 4 strings
		}
		else if (sscanf(linea, "%s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2) == 3)
		{
			strcpy(instruccion->arg3, "");
		}
		else if (sscanf(linea, "%s %s", instruccion->instruccion, instruccion->arg1) == 2)
		{
			strcpy(instruccion->arg2, "");
			strcpy(instruccion->arg3, "");
		}
		else if (sscanf(linea, "%s", instruccion->instruccion) == 1)
		{
			strcpy(instruccion->arg1, "");
			strcpy(instruccion->arg2, "");
			strcpy(instruccion->arg3, "");
		}
		printf("\ninstruccion %s\n",instruccion->instruccion);
		printf("arg1 %s\n",instruccion->arg1);
		printf("arg2 %s\n",instruccion->arg2);
		printf("arg3 %s\n",instruccion->arg3);

		list_add(instrucciones, (void *)instruccion);
	}
	free(instruccion);
	fclose(archivo_instrucciones);

	return instrucciones;
}

// para los arrays de kernel
int largo_array(char **array)
{
	int largo = 0;
	while (array[largo] != NULL)
	{
		largo++;
	}
	return largo;
}

int enviar_handshake(t_log *logger, int socket_cliente, op_code handshake)
{
	int resultado;
	send(socket_cliente, &handshake, sizeof(int), 0);
	recv(socket_cliente, &resultado, sizeof(int), MSG_WAITALL);
	if (resultado != -1)
	{
		log_info(logger, "Handshake OK");
	}
	else
	{
		log_error(logger, "Handshake rechazado");
	}

	return resultado;
}

int realizar_handshake(t_log *logger, int socket_servidor, op_code handshake)
{

	if (enviar_handshake(logger, socket_servidor, handshake) == -1)
	{
		close(socket_servidor);
		log_error(logger, "No se pudo realizar el handshake con el servidor");
		return -1;
	}

	return 0;
}

void aceptar_handshake(t_log *logger, int socket_cliente, op_code cop)
{
	int result_ok = 0;
	log_info(logger, "Recibido handshake %s.", op_code_desc[cop]);
	send(socket_cliente, &result_ok, sizeof(int), 0);
}

void rechazar_handshake(t_log *logger, int socket_cliente)
{
	int result_error = -1;
	log_error(logger, "Recibido handshake de un modulo no autorizado, rechazando...");
	send(socket_cliente, &result_error, sizeof(int), 0);
}

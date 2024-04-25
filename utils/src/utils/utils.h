#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

// Codigos de operaciones
typedef enum{
	HANDSHAKE_OK,                 // CPU DISPATCH -> KERNEL (aunque podríamos usarlo para todos?) 
    MENSAJE_A_MEMORIA,
	HANDSHAKE_KERNEL            // KERNEL -> CPU DISPATCH
} op_code;

typedef struct{
	int size;
	void* stream;
} t_buffer;

typedef struct{
    op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// Funciones de init
t_config* iniciar_config(char* ruta_config);
t_log* iniciar_logger(char* ruta_logger, char* nombre_logger);

// Funciones de sockets
int crear_conexion(char *ip, char* puerto,t_log* logger);
void liberar_conexion(int socket_cliente);
int iniciar_servidor(char* puerto, t_log* logger, char* mensaje_servidor);
int esperar_cliente(int socket_servidor, t_log* logger,char* cliente);

// Funciones de serializacion y mensajeria
//void crear_buffer(t_paquete* paquete);
t_buffer* crear_buffer();
void destruir_buffer(t_buffer* buffer);
void cargar_datos_al_buffer(t_buffer* buffer, void* datos, int size_datos);
void cargar_int_al_buffer(t_buffer* buffer, int valor_int);
void cargar_string_al_buffer(t_buffer* buffer, char* valor_string);
int recibir_operacion(int socket_cliente);
//void* recibir_buffer(int*, int);
t_buffer* recibir_buffer_completo(int socket_cliente);
void* extraer_datos_del_buffer(t_buffer* buffer);
int extraer_int_del_buffer(t_buffer* buffer);
char* extraer_string_del_buffer(t_buffer* buffer);
// Serializacion
t_paquete* crear_paquete(op_code code_op, t_buffer* buffer);
void eliminar_paquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete);
void enviar_paquete(t_paquete* paquete, int socket_cliente);

// Funciones auxiliares
int largo_array(char**array);

// Handshakes
int realizar_handshake(t_log *logger, int socket_servidor, op_code handshake);
void aceptar_handshake(t_log *logger, int socket_cliente, op_code cop);
void rechazar_handshake(t_log *logger, int socket_cliente);

#endif
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
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <time.h>

//Linea de instruccion, si no usa algun argumento,se deja empty
typedef struct
{
	char * instruccion;
	char * arg1;
	char * arg2;
	char * arg3;
} t_instruccion;
/*typedef struct
{
	char instruccion[20];
	char arg1[20];
	char arg2[20];
	char arg3[20];
} t_instruccion;*/


// Codigos de operaciones
typedef enum{
	HANDSHAKE_OK,                 // CPU DISPATCH -> KERNEL (aunque podríamos usarlo para todos?) 
    MENSAJE_A_MEMORIA,
	HANDSHAKE_KERNEL,             // KERNEL -> CPU DISPATCH
	HANDSHAKE_ENTRADASALIDA,	  
	HANDSHAKE_MEMORIA,
	HANDSHAKE_CPU,
	KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS, // PIDE A MEMORIA INICIALIZAR ESTRUCTURA Y LO HACE CORRECTAMENTE.
	MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS,
	ENVIAR_PROCESO_A_EXEC,
	CPU_INTERRUPT
} op_code;

typedef struct{
	int size;
	void* stream;
} t_buffer;

typedef struct{
    op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// COMANDOS QUE VA A UTILIZAR LA CONSOLA, SIRVE PARA VALIDACION Y EJECUCION DE COMANDOS EN KERNEL
typedef enum{
	EJECUTAR_SCRIPT = 0,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	PROCESO_ESTADO,
	MULTIPROGRAMACION,
	MENSAJE_A_MEMORIA1,
	COMANDO_INVALIDO
}comando_consola;


typedef struct
{
 uint8_t AX;
 uint8_t BX;
 uint8_t CX;
 uint8_t DX;
 uint32_t EAX;
 uint32_t EBX;
 uint32_t ECX;
 uint32_t EDX;
 uint32_t SI;
 uint32_t DI;

} t_registros_cpu;
typedef enum {
    E_NEW,
    E_READY,
    E_EXEC,
    E_BLOCKED,
    E_EXIT
} estado_pcb;

typedef struct
{
  uint32_t pid;
  uint32_t program_counter;
  t_registros_cpu *registros_cpu;
  int quantum;
  estado_pcb estado;

} t_pcb;

// MOTIVOS DE INTERRUPCION
typedef enum
{
    FIN_DE_QUANTUM,
	ENTRADA_SALIDA,
    ELIMINAR_PROCESO

} motivo_interrupcion;




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
t_pcb* recibir_pcb(int socket_cliente);
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
void *serializar_instruccion(t_instruccion *instruccion);
t_instruccion *deserializar_instruccion(void *stream);
t_list *parsear_archivo_instrucciones(char *path_archivo, t_log *logger);

// Funciones auxiliares
int largo_array(char**array);

// Handshakes
int realizar_handshake(t_log *logger, int socket_servidor, op_code handshake);
void aceptar_handshake(t_log *logger, int socket_cliente, op_code cop);
void rechazar_handshake(t_log *logger, int socket_cliente);

#endif
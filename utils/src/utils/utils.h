#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <time.h>




// Codigos de operaciones
typedef enum
{
	// HANDHAKES
	HANDSHAKE_OK, // CPU DISPATCH -> KERNEL (aunque podríamos usarlo para todos?)
	MENSAJE_A_MEMORIA,
	HANDSHAKE_KERNEL, // KERNEL -> CPU DISPATCH/interrupt
	HANDSHAKE_ENTRADASALIDA,
	HANDSHAKE_MEMORIA,
	HANDSHAKE_CPU,
	// KERNEL-MEMORIA
	KERNEL_RESPUESTA_INICIALIZAR_ESTRUCTURAS, // PIDE A MEMORIA INICIALIZAR ESTRUCTURA Y LO HACE CORRECTAMENTE.
	MEMORIA_SOLICITAR_INICIALIZAR_ESTRUCTURAS,
	LIBERAR_PROCESO_EN_MEMORIA,		 // EN ELIMINAR PROCESO KERNEL -> SOLICITA A MEMORIA LIBERAR MEMORIA DE DICHO PID
	// KERNEL-CPU
	IO_GEN_SLEEP_FS,				// le pongo _FS al final por que sino hay conflicto con la isntruccion IO_GEN_SLEEP
	IO_STDIN_READ_FS,
	IO_STDOUT_WRITE_FS,
	IO_FS_CREATE_FS,
	IO_FS_DELETE_FS,
	IO_FS_TRUNCATE_FS,
	IO_FS_WRITE_FS,
	IO_FS_READ_FS,
	KERNEL_EXIT,
	//CONTEXTO_EJECUCION,				// cpu-> KERNEL por dipatch
	KERNEL_ENVIA_PROCESO,			// KERNEL -> CPU por dispatch
	CPU_INTERRUPT,
	KERNEL_WAIT,   					// cpu-> KERNEL por dipatch
	KERNEL_SIGNAL, 					// cpu-> KERNEL por dipatch
	// CPU-MEMORIA
	CPU_SOLICITA_INSTRUCCION,  // cpu->memoria 
	MEMORIA_ENVIA_INSTRUCCION,	    // memoria->cpu 
	MEMORIA_RESIZE,				    // cpu-> MEMORIA//mano pid y new_size
	RESIZE_OK,					    // memoria ->cpu
	OUT_OF_MEMORY,				    // memoria ->cpu
	MEMORIA_LEER,				    // cpu-> MEMORIA
	MEMORIA_ESCRIBIR,			    // cpu-> MEMORIA
	CPU_CONSULTA_FRAME,			    // cpu<-> MEMORIA cuando hay tlb miss
	CPU_CONSULTA_TAM_PAGINA,	    // cpu<-> MEMORIA por que es un dato que no viene en el cpu.config y lo necesito
	INTERRUPCION,
	MEMORIA_COPY_STRING,
	MEMORIA_MOV_OUT,
	MEMORIA_MOV_IN,
	//COPY_STRING_LEER, // cpu-> MEMORIA solicito la info en la dir fisica
	//COPY_STRING_LEER_RESPUESTA,//memoria->cpu ,memoria me devuelve lo solicitado
	//COPY_STRING_ESCRIBIR, // cpu-> MEMORIA , le vuelvo a enviar la info recortada
	// IO-KERNEL
	FIN_IO,							// IO -> KERNEL
	ERROR_IO, 						// CUANDO IO NO CONTIENE FUNCION EN LA INTERFAZ, KERNEL MADNA A EXIT AL PROCESO
	NUEVA_CONEXION_IO
} op_code;

typedef struct
{
	int size;
	void *stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer *buffer;
} t_paquete;

// COMANDOS QUE VA A UTILIZAR LA CONSOLA, SIRVE PARA VALIDACION Y EJECUCION DE COMANDOS EN KERNEL
typedef enum
{
	EJECUTAR_SCRIPT = 0,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	PROCESO_ESTADO,
	MULTIPROGRAMACION,
	MENSAJE_A_MEMORIA1,
	COMANDO_INVALIDO
} comando_consola;

typedef struct
{
	//uint32_t PROGRAM_COUNTER;
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

typedef enum
{
	E_NEW,
	E_READY,
	E_EXEC,
	E_BLOCKED,
	E_EXIT,
	E_PRIORIDAD
} estado_pcb;

//
// MOTIVOS DE INTERRUPCION o FIN DE PROCESO

typedef enum
{
	FIN_DE_QUANTUM,
	ENTRADA_SALIDA,
	ELIMINAR_PROCESO,
	PROCESO_OUT_OF_MEMORY
} motivo_interrupcion;

typedef struct
{
	uint32_t pid;
	uint32_t program_counter;
	t_registros_cpu *registros_cpu;
	int quantum;
	estado_pcb *estado;
	// motivo_interrupcion motivo;
} t_pcb;

typedef struct
{
	char nombre[100];
	int socket;
	t_list* cola_espera;
	sem_t sem_espera;
	pthread_mutex_t mutex_interfaz;
} t_interfaz;


typedef enum
{
	GENERICA,
	STDIN,
	STDOUT,
	DIAL_FS
} tipo_interfaz;

// Interfaz
/*
typedef struct
{
	t_config *config;
	t_queue *cola_espera;
	pthread_t hilo;
	sem_t *semaforo;
} t_interfaz;
*/



// Funciones de init
t_config *iniciar_config(char *ruta_config);
t_log *iniciar_logger(char *ruta_logger, char *nombre_logger);

// Funciones de sockets
int crear_conexion(char *ip, char *puerto, t_log *logger);
void liberar_conexion(int socket_cliente);
int iniciar_servidor(char *puerto, t_log *logger, char *mensaje_servidor);
int esperar_cliente(int socket_servidor, t_log *logger, char *cliente);

// Funciones de serializacion y mensajeria
// void crear_buffer(t_paquete* paquete);
t_buffer *crear_buffer();
void destruir_buffer(t_buffer *buffer);
void cargar_datos_al_buffer(t_buffer *buffer, void *datos, int size_datos);
void cargar_int_al_buffer(t_buffer *buffer, int valor_int);
void cargar_string_al_buffer(t_buffer *buffer, char *valor_string);
int recibir_operacion(int socket_cliente);
char *recibir_cadena(int socket);
int recibir_int(int socket);
t_pcb *recibir_pcb(int socket_cliente);
// void* recibir_buffer(int*, int);
t_buffer *recibir_buffer_completo(int socket_cliente);
void *extraer_datos_del_buffer(t_buffer *buffer);
int extraer_int_del_buffer(t_buffer *buffer);
char *extraer_string_del_buffer(t_buffer *buffer);
// Serializacion
t_paquete *crear_paquete(op_code code_op, t_buffer *buffer);
void eliminar_paquete(t_paquete *paquete);
void *serializar_paquete(t_paquete *paquete);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
//void *serializar_instruccion(t_instruccion *instruccion);
//t_instruccion *deserializar_instruccion(void *stream);
//t_list *parsear_archivo_instrucciones(char *path_archivo, t_log *logger);
t_pcb *deserializar_pcb(t_buffer *buffer);

// Funciones auxiliares
int largo_array(char **array);

// Handshakes
int realizar_handshake(t_log *logger, int socket_servidor, op_code handshake);
void aceptar_handshake(t_log *logger, int socket_cliente, op_code cop);
void rechazar_handshake(t_log *logger, int socket_cliente);

void print_registros(t_registros_cpu *registros);
void print_pcb(t_pcb* pcb);
#endif
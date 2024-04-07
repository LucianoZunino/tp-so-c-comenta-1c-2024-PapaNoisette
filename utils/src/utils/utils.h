#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/log.h>
#include <commons/config.h>

t_config* iniciar_config(char* ruta_config);
t_log* iniciar_logger(char* ruta_logger, char* nombre_logger);
int crear_conexion(char *ip, char* puerto);
void liberar_conexion(int socket_cliente);
int iniciar_servidor(char* puerto, t_log* logger, char* mensaje_servidor);
int esperar_cliente(int socket_servidor, t_log* logger);
int recibir_operacion(int socket_cliente);


#endif
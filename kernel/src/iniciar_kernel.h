#ifndef INICIAR_KERNEL_H
#define INICIAR_KERNEL_H

#include "main.h"
#include <utils/utils.h>

void iniciar_kernel();
void iniciar_logger_kernel();
void iniciar_config_kernel();
void imprimir_config_kernel();
void iniciar_semaforos();
void iniciar_recursos();
void iniciar_colas_estados();
void finalizar_kernel();
void enviar_datos_cpu();
void esperar_clientes();
#endif
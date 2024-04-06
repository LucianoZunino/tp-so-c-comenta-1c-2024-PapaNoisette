#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>

int main(int argc, char* argv[]) {
    decir_hola("Kernel");
    t_log* logger_kernel = iniciar_logger("kernel.log", "Kernel");
    t_config* config_kernel = iniciar_config("kernel.config");

    char* ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
	char* ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
    char* puerto_escucha = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
	
	log_info(logger_kernel, "Leí la IP_MEMORIA %s, la IP_CPU %s, y el PUERTO_ESCUCHA %s\n", ip_memoria, ip_cpu, puerto_escucha);
    return 0;
}

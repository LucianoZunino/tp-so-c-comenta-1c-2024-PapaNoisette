#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

int main(int argc, char* argv[]) {
    decir_hola("cpu");
    iniciar_cpu();
  	log_info(logger_cpu, "Arranca el modulo  CPUUUU");


    return 0;
}
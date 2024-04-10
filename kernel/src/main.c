#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include "main.h"

int main(int argc, char* argv[]) {
    decir_hola("Kernel");
    iniciar_kernel();
    log_info(logger_kernel, "Arranca el modulo  KERNEL");

    return 0;
}

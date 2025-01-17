
#include "instrucciones.h"

#define SIN_INTERRUPCION -1
#define CONTINUAR_CICLO 0
#define SALIR_DE_CICLO 1

char instruccion[20];
char instr_arg1[20];
char instr_arg2[20];
char instr_arg3[20];
char instr_arg4[20];
char instr_arg5[20];
bool flag_desalojo;

pthread_mutex_t interrupcion_mutex;

/// @brief Solicita a memoria la siguiente instruccion
/// @note
/// @param t_pcb
/// @return t_instruccion
void fetch(){   
    char buffer[100];
    recv(fd_memoria, buffer, sizeof(100), MSG_DONTWAIT); //borro el buffer 
    //printf("buffer, %s\n", buffer);
    
    if(EXEC != NULL){
        log_info(logger_cpu, "Fetch Instrucción: \"PID: %i - FETCH - Program Counter: %i\"", EXEC->pid, EXEC->program_counter);

        // enviar_memoria_solicitar_instruccion(EXEC, fd_memoria);
        enviar_memoria_solicitar_instruccion(EXEC->pid, EXEC->program_counter, fd_memoria);
    }
    // la instruccion sin parsear tipo "IO_FS_READ Int4 notas.txt BX ECX EDX"
    char *cpu_respuesta_instruccion = malloc(100);

    op_code codigo_operacion = recibir_operacion(fd_memoria);
    if(codigo_operacion == MEMORIA_ENVIA_INSTRUCCION){
        t_buffer *buffer = crear_buffer();
        buffer = recibir_buffer_completo(fd_memoria);

        strcpy(cpu_respuesta_instruccion, extraer_string_del_buffer(buffer));

        destruir_buffer(buffer);
    }

    printf("\n|--------------------------------------------------\n");
    printf("| Instrucción recibida de memoria: %s\n", cpu_respuesta_instruccion);
    printf("|--------------------------------------------------\n\n");

    char **array_instruccion = string_split(cpu_respuesta_instruccion, " ");

    for(int i = string_array_size(array_instruccion); i <= 5; i++){
        string_array_push(&array_instruccion, "");
    }

    free(cpu_respuesta_instruccion);

    strcpy(instruccion, array_instruccion[0]);
    strcpy(instr_arg1, array_instruccion[1]);
    strcpy(instr_arg2, array_instruccion[2]);
    strcpy(instr_arg3, array_instruccion[3]);
    strcpy(instr_arg4, array_instruccion[4]);
    strcpy(instr_arg5, array_instruccion[5]);
}

/// @brief Procesa una instruccion para un proceso
/// @note Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar y si la misma requiere de una traducción de dirección lógica a dirección física.
/// @param t_instruccion, t_pcb
/// @return retorna un flag para sabes si sigue el ciclo o se interrumpe
int decode_excute(){
    EXEC->program_counter += 1; // lo hago aca para que en tal caso el jnz lo sobreescriba

    if(strcmp(instruccion, "SET") == 0){
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "SET", instr_arg1, instr_arg2);
        ejecutar_set(instr_arg1, atoi(instr_arg2));
        return CONTINUAR_CICLO;
    }  
    else if(strcmp(instruccion, "MOV_IN") == 0){
        ejecutar_mov_in(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "MOV_IN", instr_arg1, instr_arg2);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "MOV_OUT") == 0){
        ejecutar_mov_out(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "MOV_OUT", instr_arg1, instr_arg2);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "SUM") == 0){
        ejecutar_sum(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "SUM", instr_arg1, instr_arg2);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "SUB") == 0){
        ejecutar_sub(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "SUB", instr_arg1, instr_arg2);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "JNZ") == 0){
        ejecutar_jnz(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "JNZ", instr_arg1, instr_arg2);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "RESIZE") == 0){
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s\”", EXEC->pid, "RESIZE", instr_arg1);
        if(ejecutar_resize(instr_arg1) == 0){
            return CONTINUAR_CICLO;
        }
        else{
            
            return SALIR_DE_CICLO;
        }
    }
    else if(strcmp(instruccion, "COPY_STRING") == 0){
        ejecutar_copy_string(instr_arg1);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s\”", EXEC->pid, "COPY_STRING", instr_arg1);
        return CONTINUAR_CICLO;
    }
    else if(strcmp(instruccion, "WAIT") == 0){
        flag_desalojo = false;
        ejecutar_wait(instr_arg1);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s\”", EXEC->pid, "WAIT", instr_arg1);
        return SALIR_DE_CICLO;

    }
    else if(strcmp(instruccion, "SIGNAL") == 0){
        flag_desalojo = false;
        ejecutar_signal(instr_arg1);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s\”", EXEC->pid, "SIGNAL", instr_arg1);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_GEN_SLEEP") == 0){
        flag_desalojo = false;
        ejecutar_io_gen_sleep(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "IO_GEN_SLEEP", instr_arg1, instr_arg2);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_STDIN_READ") == 0){
        flag_desalojo = false;
        ejecutar_io_stdin_read(instr_arg1, instr_arg2, instr_arg3);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s %s\”", EXEC->pid, "IO_STDIN_READ", instr_arg1, instr_arg2, instr_arg3);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_STDOUT_WRITE") == 0){
        flag_desalojo = false;
        ejecutar_io_stdout_write(instr_arg1, instr_arg2, instr_arg3);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s %s\”", EXEC->pid, "IO_STDOUT_WRITE", instr_arg1, instr_arg2, instr_arg3);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_FS_CREATE") == 0){
        flag_desalojo = false;
        ejecutar_io_fs_create(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "IO_FS_CREATE", instr_arg1, instr_arg2);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_FS_DELETE") == 0){
        flag_desalojo = false;
        ejecutar_io_fs_delete(instr_arg1, instr_arg2);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s\”", EXEC->pid, "IO_FS_DELETE", instr_arg1, instr_arg2);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_FS_TRUNCATE") == 0){
        flag_desalojo = false;
        ejecutar_io_fs_truncate(instr_arg1, instr_arg2, instr_arg3);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s %s\”", EXEC->pid, "IO_FS_TRUNCATE", instr_arg1, instr_arg2, instr_arg3);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_FS_WRITE") == 0){
        flag_desalojo = false;
        ejecutar_io_fs_write(instr_arg1, instr_arg2, instr_arg3, instr_arg4, instr_arg5);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s %s %s %s\”", EXEC->pid, "IO_FS_WRITE", instr_arg1, instr_arg2, instr_arg3, instr_arg4, instr_arg5);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "IO_FS_READ") == 0){
        flag_desalojo = false;
        ejecutar_io_fs_read(instr_arg1, instr_arg2, instr_arg3, instr_arg4, instr_arg5);
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s - %s %s %s %s %s\”", EXEC->pid, "IO_FS_READ", instr_arg1, instr_arg2, instr_arg3, instr_arg4, instr_arg5);
        return SALIR_DE_CICLO;
    }
    else if(strcmp(instruccion, "EXIT") == 0){
        flag_desalojo = false;
        ejecutar_exit();
        log_info(logger_cpu, "Instrucción Ejecutada: \“PID: %i - Ejecutando: %s\”", EXEC->pid, "EXIT");
        return SALIR_DE_CICLO;
    }
    else{
        log_error(logger_cpu, "La intrucción no es válida");
        return -1; // supuestamente las instrucciones nunca vienen incorrectas, se deja para pruebas
    }
}

int ciclo_de_instruccion(){ // FETCH->EXECUTE

    if(tamanio_pagina == -1){
        tamanio_pagina = consultar_tamanio_pagina_memoria();//es un dato necesario en cpu y no viene en config
    }
    // Recibo instruccion y la mando a ejecutar
    int estado_ciclo = 1; 
    // 0 continuar 1 salir de ciclo,incializo en 1 para que no loopee eternamente

    fetch(); 

    estado_ciclo = decode_excute();

    // if(flag_desalojo){
    //     printf("\nCAMBIO ESTADO_CICLO POR FLAG\n");
    //     estado_ciclo = SALIR_DE_CICLO;
    //     printf("\nASIGNA SALIR_DE_CICLO\n");
    //     sem_post(&sem_interrupt);
    //     printf("\nABAJO DE SEM_POST\n");
    // }

    /*
    pthread_mutex_lock(&interrupcion_mutex);

    if(pid_interrupcion == EXEC->pid){
        estado_ciclo = SALIR_DE_CICLO;
        pid_interrupcion = SIN_INTERRUPCION;
        pthread_mutex_unlock(&interrupcion_mutex);
        motivo_interrupcion motivo = motivo;
        enviar_kernel_interrupt(EXEC,motivo, fd_kernel_dispatch);
    }
    else{
        if(pid_interrupcion != SIN_INTERRUPCION){
            log_debug(logger_cpu, "Hubo interrupcion pero para otro pid. PID en ejeucion: %i - PID de interrupcion: %i", EXEC->pid, pid_interrupcion);
            pthread_mutex_unlock(&interrupcion_mutex);
        }
    }
*/
    return estado_ciclo;
}

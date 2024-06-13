#ifndef MMU_H
#define MMU_H
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <utils/utils.h>
#include "math.h"
#include "main.h"

#include <commons/collections/list.h>

typedef struct{
    int pid;
    int pagina;
    int marco;
    //se puede agregar otra variable segun enunciado
} t_entrada_tlb;
    

int calcular_numero_de_pagina(int dir_logica);//OK TEST
int calcular_desplazamiento(int dir_logica, int numero_pagina);//OK TEST
int consultar_tamanio_pagina_memoria();
void agregar_entrada_tlb(t_entrada_tlb *entrada);//OK TEST
int buscar_marco_tlb(int pid, int dir_logica);
int obtener_marco_en_memoria(int pid, int pagina);


#endif
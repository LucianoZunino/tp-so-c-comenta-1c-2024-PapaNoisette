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
    

double calcular_numero_de_pagina(int dir_logica);//OK TEST
int calcular_desplazamiento(int dir_logica, int numero_pagina);//OK TEST
uint32_t calculo_dir_fisica(int marco, int offset);
void agregar_entrada_tlb(int pid,int pagina,int marco);
bool marco_en_tlb(int pid, int pagina, int* marco);
void actualizar_tlb_lru(int index);
uint32_t traducir_direccion_logica(int dir_logica);


/*void agregar_entrada_tlb(t_entrada_tlb *entrada);//OK TEST
int buscar_marco_tlb(int pid, int dir_logica);
int obtener_marco_en_memoria(int pid, int pagina);

//t_dir_fisica* traducir_direccion_logica(int dir_logica); // NUEVO DE LUCHO - AGREGAR
int traducir_direccion_logica(int a); // VIEJO DE NACHO - BORRAR
*/
#endif
#ifndef APTIME_H_GUARD
#define APTIME_H_GUARD

#include <time.h>

/**
 * Função que inicializa o buffer indicado com uma estrutura timespec
 * que contém o tempo de chamada da função
 * @param spec      apontador ao buffer onde vai ser guardado o tempo
*/
void getcurrenttime(struct timespec* spec);

/**
 * Função que recebe uma estrutura timespec e converte para tempo no 
 * formato raw (inteiro positivo contado desde 1 Jan 1970)
 * @param spec      estrutura timespec a ser convertida
 * @return          long long int que corresponde aos segundos
*/
long long convert_raw(struct timespec* spec);


/**
 * Função que recebe uma estrutura timespec e converte para uma string
 * de tipo yyyy-mm-dd hh:mm:ss:ms
 * @param spec      estrutura timespec a ser convertida
 * @param str       apontador ao buffer onde vai ser guardado a string
 * @param size      tamanho do buffer
*/
void convert_localtime(struct timespec* spec, char* str, int size);

#endif
#ifndef APTIME_H_GUARD
#define APTIME_H_GUARD

#include <time.h>

/* Função que atualiza spec com o tempo atual */
void set_current_time(struct timespec* spec);

/* Função que converte os tempos em spec para um formato raw (em nanosegundos) */
long long convert_raw(struct timespec* spec);

/* Função que recebe um rawtime e um formato, devolvendo uma representação string da data no dado formato */
char* get_datetime_string_from_rawtime(time_t rawtime, char* format);

/* Função que recebe um timespec e devolve a sua representação string na forma yyyy-mm-dd hh:mm:ss:ms */
char* get_datetime_string_from_spec(struct timespec* spec);

/* Função que devolve a representação string da data atual na forma yyyy-mm-dd hh:mm:ss:ms */
char* get_current_datetime_string();

/* Função que calcula a diferença entre dois timespec e guarda a representação string dessa diferença em str */
void calculate_difference(struct timespec* t1, struct timespec* t2, char* str);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_CLOCKGETTIME "Set Current Time"

// Error messages
#define ERROR_CLOCKGETTIME "Error: Failed to get current time using clock_gettime."
#endif
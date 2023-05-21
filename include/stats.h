#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include <stdio.h>
#include "main.h"

struct StatsFile {
    FILE* ptr;
};

/* Função que inicializa a estrutura de stats */
struct StatsFile* STATS_INIT(char* filename);

/* Função que destrói um stats*/
void STATS_FREE(struct StatsFile* logger);

/* Função que escreve estatísticas no data->stats_filename */
void write_stats(struct main_data* data, int op_counter);

/* Função que escreve o conteúdo do ficheiro de estatística */
void write_content(struct main_data* data, int op_counter, FILE* fpointer);

/* Função que escreve o número de operações processadas por cada entidade (client, interm, enterp) no ficheiro destino */
void write_processed_operations(struct main_data* data, FILE* fpointer);

/* Função que escreve estatísticas de uma operação no ficheiro destino */
void write_operation_statistics(struct operation op, FILE* fpointer);

// ====================================================================================================
//                                              AUXILIARY
// ====================================================================================================
#define STATS_START         "Process Statistics:\n"
#define STATS_CLIENTS       "      Client %d received %d operation(s)!\n"
#define STATS_INTERM        "      Intermediary %d prepared %d operation(s)!\n"
#define STATS_ENTERP        "      Enterprise %d executed %d operation(s)!\n"
#define STATS_REQUESTS      "\nRequest Statistics:\n"
#define STATS_REQUEST       "Request: %d\n"\
                            "Status: %c\n"
#define STATS_REQUEST_IDS   "Client id: %d\n"\
                            "Intermediary id: %d\n"\
                            "Enterprise id: %d\n"
#define STATS_REQUEST_TIMES "Start time: %s\n"\
                            "Client time: %s\n"\
                            "Intermediary time: %s\n"\
                            "Enterprise time: %s\n"\
                            "Total time: %s\n\n"


// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_STATS "Init statistics file"

// Information messages
#define INFO_LOADED_STATSFILE "\033[1;32m[+]\033[0m \033[1;36m%s\033[0m was successfully loaded as stats file!\033[0m\n"

// Error messages
#define ERROR_FAILED_OPEN_STATSFILE "\033[0;31m[!] Error:\033[0m Failed to create file.\n" 

#endif
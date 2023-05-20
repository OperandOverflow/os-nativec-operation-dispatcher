#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "main.h"

void write_stats(struct main_data* data, int op_counter);

void open_file(char* filename);

void write_content(struct main_data* data, int op_counter);

void close_file();


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

// Error handling constants
#define ERROR_FAILED_OPEN_FILE "\033[0;31m[!] Error:\033[0m Failed to create file.\n" 

#define EXIT_OPEN_FILE_ERROR 1

#endif
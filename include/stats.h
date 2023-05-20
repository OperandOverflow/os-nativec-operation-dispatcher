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

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_STATS "Init statistics file"

// Error handling constants
#define ERROR_FAILED_OPEN_FILE "\033[0;31m[!] Error:\033[0m Failed to create file.\n" 

#define EXIT_OPEN_FILE_ERROR 1

#endif
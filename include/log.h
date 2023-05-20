/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdio.h>

struct LoggingFile {
    FILE* ptr;
};

/* Função que inicializa a estrutura de logging */
struct LoggingFile* LOG_INIT(char* filename);

/* Função que escreve a mensagem no ficheiro de logging */
void ADMPOR_LOG(struct LoggingFile* logger, char* message);

/* Função que destrói um Logger*/
void LOG_FREE(struct LoggingFile* logger);




// Information messages
#define INFO_LOADED_LOGFILE "\033[1;32m[+]\033[0m \033[1;36m%s\033[0m was successfully loaded as log file!\033[0m\n"

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_LOGFILE "Init Log file"

#define ERROR_LOGFILE_ALREADY_EXISTS "Error: Unable to create log file, since the log file is already open.\n"
#define ERROR_LOGFILE_OPEN "Error: Unable to open log file.\n"
#define ERROR_LOGFILE_NOT_ACTIVE "Error: Log file is not properly set (open)."

#define EXIT_LOGFILE_ALREADY_EXISTS_ERROR 121
#define EXIT_LOGFILE_OPEN_ERROR 122

#endif
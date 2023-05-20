/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aptime.h"
#include "memory.h"
#include "main-private.h"
#include "log.h"



struct LoggingFile* LOG_INIT(char* filename) {
    struct LoggingFile* logger = (struct LoggingFile*)create_dynamic_memory(sizeof(struct LoggingFile));
    if (logger != NULL) {
        logger->ptr = fopen(filename, "a+");

        // verify if there was a problem during file opening
        verify_condition(
            logger->ptr == NULL,
            INIT_LOGFILE,
            ERROR_LOGFILE_OPEN,
            EXIT_LOGFILE_OPEN_ERROR
        );
        printf(INFO_LOADED_LOGFILE, filename);
    }
    return logger;
}

void ADMPOR_LOG(struct LoggingFile* logger, char* message) {
    // verify if logger is active
    if (logger == NULL) {
        perror(ERROR_LOGFILE_NOT_ACTIVE);
        return;
    }    
    char *timestamp = getCurrentTimeStr("%Y-%m-%d %H:%M:%S");   
    // write message
    fprintf(logger->ptr, "%s %s\n", timestamp, message);
    fflush(logger->ptr);
}

void LOG_FREE(struct LoggingFile* logger) {
    if (logger == NULL)
        return;
    
    fclose(logger->ptr);
    destroy_dynamic_memory(logger);
}
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
#include "log.h"



struct LoggingFile* LOG_INIT(char* filename) {
    // verify if filename is short enough
    if (strlen(filename) > FILENAME_MAX_SIZE) {
        perror(ERROR_LOGFILE_FILENAME_EXCEDEED);
        exit(EXIT_FILENAME_EXCEDEED_ERROR);
    }

    struct LoggingFile* logger = (struct LoggingFile*)create_dynamic_memory(sizeof(struct LoggingFile));
    if (logger != NULL) {
        strcpy(logger->filename, filename);
        logger->ptr = fopen(logger->filename, "a+");

        // verify if there was a problem during file opening
        if (logger->ptr == NULL) {
            perror(ERROR_LOGFILE_OPEN);
            exit(EXIT_LOGFILE_OPEN_ERROR);
        }
        printf(INFO_LOADED_LOGFILE, logger->filename);
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
    logger->ptr = NULL;
}
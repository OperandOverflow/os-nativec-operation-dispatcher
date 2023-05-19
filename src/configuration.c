/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "memory.h"
#include "main-private.h"


struct ConfigurationFile* config_file_init(char* filename) {
    struct ConfigurationFile* config_file;
    config_file = (struct ConfigurationFile*)create_dynamic_memory(sizeof(struct ConfigurationFile));
    if (config_file != NULL) {
        config_file->name = (char *)create_dynamic_memory(strlen(filename));
        if (config_file->name != NULL)
            strcpy(config_file->name, filename);
    }
    return config_file;
}

void config_file_free(struct ConfigurationFile* config_file) {
    free(config_file->name);
    free(config_file);
}

int load_config_file(struct ConfigurationFile* config_file, struct main_data* data) {
    // Open the file (reader mode)
    config_file->ptr = fopen(config_file->name, "r");

    verify_condition(
        config_file->ptr == NULL,
        INIT_LOAD_CONFIGFILE,
        ERROR_CONFIGFILE_OPEN,
        EXIT_CONFIGFILE_OPEN_ERROR
    );
    char line[100];
    int line_number = 0;

    while (fgets(line, sizeof(line), config_file->ptr) != NULL) {
        line_number++;
        // Trim newline character at the end of the line
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        if (line_number > CONFIG_FILE_EXPECTED_LINE_COUNT)
            return 1;

        if (line_number == 1)
            data->max_ops = atoi(line);
        else if (line_number == 2)
            data->buffers_size = atoi(line);
        else if (line_number == 3)
            data->n_clients = atoi(line);
        else if (line_number == 4)
            data->n_intermediaries = atoi(line);
        else if (line_number == 5)
            data->n_enterprises = atoi(line);
        else if (line_number == 6) {
            char* filename = (char*)create_dynamic_memory(strlen(line));
            strcpy(filename, line);
            data->log_filename = filename;
        }
        else if (line_number == 7) {
            char* filename = (char*)create_dynamic_memory(strlen(line));
            strcpy(filename, line);
            data->statistics_filename = filename;
        }
        else
            data->alarm_time = atoi(line);   
    }
    // Close the file
    fclose(config_file->ptr);

    verify_condition(
        line_number != CONFIG_FILE_EXPECTED_LINE_COUNT,
        INIT_LOAD_CONFIGFILE,
        ERROR_CONFIGFILE_NUMBER_OF_LINES,
        EXIT_CONFIGFILE_NUMBER_OF_LINES_ERROR
    );

    // File is correctly defined
    printf(INFO_LOADED_CONFIGFILE, config_file->name);
    return 0;
}
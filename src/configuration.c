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


struct ConfigurationFile* CONFIG_INIT(char* filename) {
    struct ConfigurationFile* config_file = (struct ConfigurationFile*)create_dynamic_memory(sizeof(struct ConfigurationFile));
    if (config_file != NULL) {
        config_file->ptr = fopen(filename, "r");

        // verify if there was a problem during file opening
        assert_error(
            config_file->ptr == NULL,
            INIT_LOAD_CONFIGFILE,
            ERROR_CONFIGFILE_OPEN
        );
    }
    return config_file;
}

void CONFIG_FREE(struct ConfigurationFile* config_file) {
    if (config_file && config_file->ptr)
        fclose(config_file->ptr);
    destroy_dynamic_memory(config_file);
}

int CONFIG_LOAD(struct ConfigurationFile* config_file, struct main_data* data, char* config_filename) {
    // verify if config file is open
    if (config_file == NULL) {
        printf(ERROR_CONFIGFILE_NOT_ACTIVE);
        return -1;
    }  

    char line[100];
    int line_number = 0;

    while (fgets(line, sizeof(line), config_file->ptr) != NULL) {
        line_number++;
        // skip processing, if we already processed the required lines
        if (line_number > CONFIG_FILE_EXPECTED_LINE_COUNT)
            continue;

        // trim newline character at the end of the line
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        // parse and set value to main_data struct with regard to line number
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
            char* filename = (char*)create_dynamic_memory(strlen(line) + 1);
            strcpy(filename, line);
            data->log_filename = filename;
        }
        else if (line_number == 7) {
            char* filename = (char*)create_dynamic_memory(strlen(line) + 1);
            strcpy(filename, line);
            data->statistics_filename = filename;
        }
        else if (line_number == 8)
            data->alarm_time = atoi(line);   
    }

    return line_number;
}

void parse_config_file(char* config_filename, struct main_data* data) {
    struct ConfigurationFile* config_file = CONFIG_INIT(config_filename);
    // vefify if config file is initialized
    if (!config_file || !config_file->ptr) {
        data->buffers_size = -1;
        CONFIG_FREE(config_file);
        return;        
    }

    int loadedLines = CONFIG_LOAD(config_file, data, config_filename);

    // verify if the configuration file is missing required fields (set buffer size to -1)
    if (assert_error(loadedLines < CONFIG_FILE_EXPECTED_LINE_COUNT, INIT_LOAD_CONFIGFILE, ERROR_CONFIGFILE_MISSING_REQUIRED_FIELDS)) {
        data->buffers_size = -1;
        CONFIG_FREE(config_file);
        return;
    }

    // show a warning if the parsing ignored lines
    if (loadedLines > CONFIG_FILE_EXPECTED_LINE_COUNT)
        printf(WARNING_CONFIGFILE_NUMBER_OF_LINES, loadedLines, CONFIG_FILE_EXPECTED_LINE_COUNT);

    printf(INFO_LOADED_CONFIGFILE, config_filename);

    // release resources
    CONFIG_FREE(config_file);


}
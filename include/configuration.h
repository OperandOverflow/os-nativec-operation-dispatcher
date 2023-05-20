/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include "main.h"

#define CONFIG_FILE_EXPECTED_LINE_COUNT 8

struct ConfigurationFile {
    char* name;
    FILE* ptr;
};

/* Função que devolve um apontar para um ConfigurationFile, criado a partir do filename dado */
struct ConfigurationFile* config_file_init(char* filename);

/* Função que liberta memória alocada pelo dado config_file */
void config_file_free(struct ConfigurationFile* config_file);

/* Função que lê os camopos definidos no config_file, populando os campos de data */
int load_config_file(struct ConfigurationFile* config_file, struct main_data* data);

// Sections
#define INIT_LOAD_CONFIGFILE "Load Config File"

// Information messages
#define INFO_LOADED_CONFIGFILE "\033[1;32m[+]\033[0m \033[1;36m%s\033[0m was successfully loaded as configuration file!\033[0m\n"

// Error handling constants
#define ERROR_CONFIGFILE_OPEN "Error: Failed to open the file.\n"
#define ERROR_CONFIGFILE_NUMBER_OF_LINES "Error: Unexpected number of lines in the file.\n"

#define EXIT_CONFIGFILE_OPEN_ERROR 81
#define EXIT_CONFIGFILE_NUMBER_OF_LINES_ERROR 82


#endif
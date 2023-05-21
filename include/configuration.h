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
    FILE* ptr;
};

/* Função que devolve um apontar para um ConfigurationFile, criado a partir do filename dado */
struct ConfigurationFile* CONFIG_INIT(char* filename);

/* Função que liberta memória alocada pelo dado config_file */
void CONFIG_FREE(struct ConfigurationFile* config_file);

/* Função que lê os campos definidos no config_file, populando os campos de data */
int CONFIG_LOAD(struct ConfigurationFile* config_file, struct main_data* data, char* config_filename);

/* Função que atualiza main_data com os valores no ficheiro config_filename. data->buffer_size fica a -1 em caso de erro */
void parse_config_file(char* config_filename, struct main_data* data);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_LOAD_CONFIGFILE "Load Config File"

// Information messages
#define INFO_LOADED_CONFIGFILE "\033[1;32m[+]\033[0m \033[1;36m%s\033[0m was successfully loaded as configuration file!\033[0m\n"

// Warning messages
#define WARNING_CONFIGFILE_NUMBER_OF_LINES "Warning: Unexpected number of lines in the configuration file (%d != %d).\n"

// Error handling constants
#define ERROR_CONFIGFILE_OPEN "Error: Failed to open the configuration file.\n"
#define ERROR_CONFIGFILE_NOT_ACTIVE "Error: Configuration file is not properly set (open).\n"
#define ERROR_CONFIGFILE_MISSING_REQUIRED_FIELDS "Error: Configuration file is missing required fields.\n"

#endif
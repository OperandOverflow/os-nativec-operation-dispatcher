/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef MEMORY_PRIVATE_H_GUARD
#define MEMORY_PRIVATE_H_GUARD

#include "main.h"

#define FREE_MEM 0
#define USED_MEM 1

/* Função que liberta memória dinamica de uma estrutura main data */
void main_data_dynamic_memory_free(struct main_data* data);

/* Função que liberta memória dinamica de uma estrutura comm_buffers */
void comm_buffers_dynamic_memory_free(struct comm_buffers* buffers);

// Função para limpar buffer de input do utilizador
void flush();

/* Função que liberta memória se o apontador não for NULL */
void safe_free(void* ptr);

// Funcao que escreve uma dada operacao em um dado buffer de tamanho buffer_size
void write_operation_to_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Error messages
#define ERROR_SHM_OPEN "\033[0;31m[!] Error:\033[0m Failed to open shared memory region.\n"
#define ERROR_SHM_TRUNCATE "\033[0;31m[!] Error:\033[0m Failed to truncate shared memory region.\n"
#define ERROR_SHM_MAP "\033[0;31m[!] Error:\033[0m Failed to map shared memory region.\n"
#define ERROR_SHM_UNMAP "\033[0;31m[!] Error:\033[0m Failed to unmap shared memory region.\n"
#define ERROR_SHM_UNLINK "\033[0;31m[!] Error:\033[0m Failed to unlink shared memory region.\n"

#endif
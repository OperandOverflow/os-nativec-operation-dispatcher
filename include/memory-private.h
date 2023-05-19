/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef MEMORY_PRIVATE_H_GUARD
#define MEMORY_PRIVATE_H_GUARD

#define FREE_MEM 0
#define USED_MEM 1

// Funcao que escreve uma dada operacao em um dado buffer de tamanho buffer_size
void write_operation_to_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);

// Error handling constants
#define ERROR_SHM_OPEN "\033[0;31m[!] Error:\033[0m Failed to open shared memory region.\n"
#define ERROR_SHM_TRUNCATE "\033[0;31m[!] Error:\033[0m Failed to truncate shared memory region.\n"
#define ERROR_SHM_MAP "\033[0;31m[!] Error:\033[0m Failed to map shared memory region.\n"
#define ERROR_SHM_UNMAP "\033[0;31m[!] Error:\033[0m Failed to unmap shared memory region.\n"
#define ERROR_SHM_UNLINK "\033[0;31m[!] Error:\033[0m Failed to unlink shared memory region.\n"


#define EXIT_SHM_OPEN_ERROR 10
#define EXIT_SHM_TRUNCATE_ERROR 11
#define EXIT_SHM_MAP_ERROR 12
#define EXIT_MUNMAP_ERROR 13
#define EXIT_SHM_UNLINK_ERROR 14

#endif
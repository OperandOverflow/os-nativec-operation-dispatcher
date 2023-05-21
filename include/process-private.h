/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/


#ifndef PROCESS_PRIVATE_H_GUARD
#define PROCESS_PRIVATE_H_GUARD

/* Função que liberta a memória alocada em um processo filho do AdmPor */
void admpor_child_process_free(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

/* type ExecutionFunc representa um ponteiro para uma funcao de execução de entidade (cliente, interm, enterp) */ 
typedef int (*ExecuteFunction)(int, struct comm_buffers*, struct main_data*, struct semaphores*);

/* Função que cria um processo filho que executa a ExecuteFunction passada, representando um entidade (client, interm, enterp) */
int launch_entity(int process_id, ExecuteFunction execute_fn, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_SEMAPHORES "Init semaphores"

// Error messages
#define ERROR_PROCESS_WAITPID "Error: Failed to wait process %d.\n"
#define ERROR_PROCESS_EXIT_NORMALLY "Error: Child process %d did not terminate normally.\n"

#endif
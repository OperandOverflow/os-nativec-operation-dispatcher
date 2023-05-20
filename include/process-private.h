#ifndef PROCESS_PRIVATE_H_GUARD
#define PROCESS_PRIVATE_H_GUARD

/* Função que liberta a memória alocada em um processo filho do AdmPor */
void admpor_child_process_free(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
#define ERROR_PROCESS_WAITPID "Error: Failed to wait process %d.\n"
#define ERROR_PROCESS_EXIT_NORMALLY "Child process %d did not terminate normally.\n"

#endif
/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/


#include "synchronization.h"

#ifndef SYNCHRONIZATION_PRIVATE_H_GUARD
#define SYNCHRONIZATION_PRIVATE_H_GUARD


/* Function that verifies if given prodcons is valid */
int is_prodcons_valid(struct prodcons* pc);

/* Function that verifies if given semaphores struct are valid */
int are_semaphores_valid(struct semaphores* sems);

/**
 * Function that undoes the consumer_begin when nothing was consumed
*/
void consume_begin_undo(struct prodcons* pc);

// Error handling constants
#define ERROR_SEM_CREATE    "Error: Failed to create the semaphores. Destroying all created semaphores and exiting.\n"

#define EXIT_SEM_CREATE_ERROR   81

#endif
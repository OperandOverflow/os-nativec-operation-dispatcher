#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#include "synchronization.h"
#include "synchronization-private.h"
#include "memory.h"


int is_prodcons_valid(struct prodcons* pc) {
    return pc->empty && pc->full && pc->mutex;
}

int are_semaphores_valid(struct semaphores* sems) {
    return is_prodcons_valid(sems->client_interm)
        && is_prodcons_valid(sems->interm_enterp)
        && is_prodcons_valid(sems->main_client)
        && sems->results_mutex;
}


/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getuid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value) {
    // unlink with possible previous semaphore
    sem_unlink(name);
    uid_t uid = getuid();
    // add process id to the name
    char nameWithId[strlen(name) + 12];
    sprintf(nameWithId, "/%s_%d", name, uid);
    // create semaphore
    sem_t* sem = sem_open(name, O_CREAT | O_EXCL, 0xFFFFFFFF, value); 
    if (sem == SEM_FAILED) {
        return NULL; // return null if creation failed
    }
    return sem;
}

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore) {
    uid_t uid = getuid();
    // add process id to the name
    char nameWithId[strlen(name) + 12];
    sprintf(nameWithId, "/%s_%d", name, uid);
    
    // close
    sem_close(semaphore);
    // unlink
    sem_unlink(name);
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc) {
    // wait for empty space in buffer
    semaphore_mutex_lock(pc->empty);
    // adquire write permission
    semaphore_mutex_lock(pc->mutex);
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc) {
    // release write permission
    semaphore_mutex_unlock(pc->mutex);
    // inform consumers
    semaphore_mutex_unlock(pc->full);
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc) {
    // wait for any data to consume
    semaphore_mutex_lock(pc->full);
    // adquire read permission
    semaphore_mutex_lock(pc->mutex);
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc) {
    // release read permission
    semaphore_mutex_unlock(pc->mutex);
    // inform producers
    semaphore_mutex_unlock(pc->empty);
}

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem) {
    sem_wait(sem);
}

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem) {
    sem_post(sem);
}

/**
 * Função que faz o processo inverso de consume_begin para o 
 * caso de nada foi consumido no buffer
*/
void consume_begin_undo(struct prodcons* pc) {
    // release read permission
    sem_post(pc->mutex);
    // undo consume buffer
    sem_post(pc->full);
}
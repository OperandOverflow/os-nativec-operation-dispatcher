/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef SYNCHRONIZATION_H_GUARD
#define SYNCHRONIZATION_H_GUARD

#include <semaphore.h>
	
// Nomes usados na criacao dos semaforos
#define STR_SEM_MAIN_CLIENT_FULL  "sem_main_client_full"
#define STR_SEM_MAIN_CLIENT_EMPTY "sem_main_client_empty"
#define STR_SEM_MAIN_CLIENT_MUTEX "sem_main_client_mutex"

#define STR_SEM_CLIENT_INTERM_FULL 	"sem_client_interm_full"
#define STR_SEM_CLIENT_INTERM_EMPTY "sem_client_interm_empty"
#define STR_SEM_CLIENT_INTERM_MUTEX "sem_client_interm_mutex"

#define STR_SEM_INTERM_ENTERP_FULL	"sem_interm_enterp_full"
#define STR_SEM_INTERM_ENTERP_EMPTY "sem_interm_enterp_empty"
#define STR_SEM_INTERM_ENTERP_MUTEX "sem_interm_enterp_mutex"

#define STR_SEM_RESULTS_MUTEX	"sem_results_mutex"   //v2

//estrutura de 3 semáforos utilizada no modelo produtor/consumidor
struct prodcons {
	sem_t *full, *empty, *mutex;
};

//estrutura que agrega informação de todos os semáforos necessários
struct semaphores {
	struct prodcons *main_client;	// semáforos para acesso ao buffer entre a main e clientes
	struct prodcons *client_interm; // semáforos para acesso ao buffer entre clientes e intermediários
	struct prodcons *interm_enterp; 	// semáforos para acesso ao buffer entre intermediários e empresas
	sem_t *results_mutex;		//v2 semáforo para exclusão mútua no acesso ao array de resultados
};

/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getuid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value);

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore);

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc);

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc);

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc);

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc);

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem);

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem);


#endif

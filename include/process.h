/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef PROCESS_H_GUARD
#define PROCESS_H_GUARD

#include "memory.h"
#include "main.h"

/* Função que inicia um novo processo cliente através da função fork do SO. O novo
* processo irá executar a função execute_client respetiva, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_client(int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que inicia um novo processo intermediário através da função fork do SO. O novo
* processo irá executar a função execute_intermediary, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_interm(int interm_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que inicia um novo processo empresa através da função fork do SO. O novo
* processo irá executar a função execute_enterprise, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_enterp(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que espera que um processo termine através da função waitpid. 
* Devolve o retorno do processo, se este tiver terminado normalmente.
*/
int wait_process(int process_id);

#endif

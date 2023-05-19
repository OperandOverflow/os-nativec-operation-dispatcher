/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef INTERMEDIARY_H_GUARD
#define INTERMEDIARY_H_GUARD

#include "memory.h"
#include "main.h"

/* Função principal de um Intermediário. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação dos clientes e se a mesma tiver id 
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* envia a mesma para as empresas. Operações com id igual a -1 são 
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi 
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em intermediary.h.
*/
int execute_intermediary(int interm_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que lê uma operação do buffer de memória partilhada entre clientes e intermediários.
* Antes de tentar ler a operação, deve verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void intermediary_receive_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que processa uma operação, alterando o seu campo receiving_intermediary para o id
* passado como argumento, alterando o estado da mesma para 'I' (intermediary), e 
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void intermediary_process_operation(struct operation* op, int interm_id, struct main_data* data, int* counter, struct semaphores* sems);


/* Função que escreve uma operação no buffer de memória partilhada entre
* intermediários e empresas.
*/
void intermediary_send_answer(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

#endif

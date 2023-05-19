/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef ENTERPRISE_H_GUARD
#define ENTERPRISE_H_GUARD

#include "memory.h"
#include "main.h"

/* Função principal de uma Empresa. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação e se e data->terminate ainda for igual a 0, processa-a.
* Operações com id igual a -1 são ignoradas (op inválida) e se data->terminate for igual
* a 1 é porque foi dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os outros
* métodos auxiliares definidos em enterprise.h.
*/
int execute_enterprise(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que lê uma operação do buffer de memória partilhada entre
* os intermediários e as empresas que seja direcionada à empresa enterprise_id.
* Antes de tentar ler a operação, o processo deve verificar se data->terminate
* tem valor 1. Em caso afirmativo, retorna imediatamente da função.
*/
void enterprise_receive_operation(struct operation* op, int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);


/* Função que processa uma operação, alterando o seu campo receiving_enterp para o id
* passado como argumento, alterando o estado da mesma para 'E' ou 'A' conforme o número
* máximo de operações já tiver sido atingido ou não, e incrementando o contador de operações.
* Atualiza também a operação na estrutura data.
*/
void enterprise_process_operation(struct operation* op, int enterp_id, struct main_data* data, int* counter, struct semaphores* sems);

#endif

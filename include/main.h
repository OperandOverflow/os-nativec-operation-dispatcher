/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

#include "memory.h"
#include "synchronization.h"

//Estrutura que agrega a informação necessária pela main do AdmPor.
struct main_data {
	int max_ops;			//número máximo de operações
	int buffers_size;		//tamanho máximo dos buffers de mem. partilhada
	
	int n_clients;			//número de clientes
	int n_intermediaries;	//número de intermediários
	int n_enterprises;		//número de empresas
	
	int *client_pids;			//process ids de clientes
	int *intermediary_pids;		//process ids de intermediários
	int *enterprise_pids;		//process ids de empresas
	
	int* client_stats;			//nº de operações encaminhadas por cada clientes
	int* intermediary_stats;	//nº de operações respondidas por cada intermediário
	int* enterprise_stats;		//nº de operações recebidas por cada empresa
	
	struct operation* results;	//array com histórico de ops executadas
	
	int* terminate; //flag booleana, valor 1 indica que AdmPor  deve terminar a sua execução

	char* log_filename;
	char* statistics_filename;
	char* alarm_time;
};


/* Função que lê o argumento da aplicação, nomeadamente o ficheiro de configuração
* com o número máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, e o número de clientes, de intermediários e de
* empresas, entre outros. Guarda esta informação nos campos apropriados da
* estrutura main_data.
*/
void main_args(int argc, char* argv[], struct main_data* data);

/* Função que reserva a memória dinâmica necessária para a execução
* do AdmPor, nomeadamente para os arrays *_pids e *_stats da estrutura 
* main_data. Para tal, pode ser usada a função create_dynamic_memory.
*/
void create_dynamic_memory_buffers(struct main_data* data);

/* Função que reserva a memória partilhada necessária para a execução do
* AdmPor. É necessário reservar memória partilhada para todos os buffers da
* estrutura comm_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory. O array data->results
* deve ser limitado pela constante MAX_RESULTS.
*/
void create_shared_memory_buffers(struct main_data* data, struct comm_buffers* buffers);

/* Função que inicia os processos dos clientes, intermediários e
* empresas. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* op - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do AdmPor através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

/* Cria uma nova operação identificada pelo valor atual de op_counter e com os 
* dados introduzidos pelo utilizador na linha de comandos, escrevendo a mesma 
* no buffer de memória partilhada entre main e clientes. Imprime o id da 
* operação e incrementa o contador de operações op_counter. Não deve criar 
* mais operações para além do tamanho do array data->results.
*/
void create_request(int* op_counter, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems);

/* Função que lê um id de operação do utilizador e verifica se a mesma é valida.
* Em caso afirmativo imprime informação da mesma, nomeadamente o seu estado, o 
* id do cliente que fez o pedido, o id da empresa requisitada, e os ids do cliente,
* intermediário, e empresa que a receberam e processaram.
*/
void read_status(struct main_data* data, struct semaphores* sems);

/* Função que termina a execução do programa AdmPor. Deve começar por 
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente 
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems);

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo clientes, intermediários e empresas. Para tal, pode usar a função 
* wait_process do process.h.
*/
void wait_processes(struct main_data* data);

/* Função que imprime as estatisticas finais do AdmPor, nomeadamente quantas
* operações foram processadas por cada cliente, intermediário e empresa.
*/
void write_statistics(struct main_data* data);

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct comm_buffers* buffers);

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com
* valor igual a 1. Para tal pode ser usada a função semaphore_create.*/
void create_semaphores(struct main_data* data, struct semaphores* sems);

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao
* máximo de processos que possam lá estar.*/
void wakeup_processes(struct main_data* data, struct semaphores* sems);

/* Função que liberta todos os semáforos da estrutura semaphores. */
void destroy_semaphores(struct semaphores* sems);


#endif

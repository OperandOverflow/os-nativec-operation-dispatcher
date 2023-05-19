/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <time.h>

#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

#define STR_SHM_MAIN_CLIENT_PTR 		"SHM_MAIN_CLIENT_PTR"
#define STR_SHM_MAIN_CLIENT_BUFFER 		"SHM_MAIN_CLIENT_BUFFER"
#define STR_SHM_CLIENT_INTERM_PTR 		"SHM_CLIENT_INTERM_PTR"
#define STR_SHM_CLIENT_INTERM_BUFFER 	"SHM_CLIENT_INTERM_BUFFER"
#define STR_SHM_INTERM_ENTERP_PTR 		"SHM_INTERM_ENTERP_PTR"
#define STR_SHM_INTERM_ENTERP_BUFFER 	"SHM_INTERM_ENTERP_BUFFER"
#define STR_SHM_RESULTS					"SHM_RESULTS"
#define STR_SHM_TERMINATE				"SHM_TERMINATE"

#define MAX_RESULTS 100		//constante usada para limitar tamanho de buffer de resultados

struct pointers { 																					
	int in;  																						
	int out; 																						
};																								

//estrutura que representa um buffer circular
struct circular_buffer { 	
	struct pointers *ptrs;
	struct operation* buffer;
};

//estrutura que representa um buffer de acesso aleatório
struct rnd_access_buffer {
	int* ptrs;
	struct operation* buffer;
};


//Estrutura que representa uma operação (pedido/resposta)
struct operation {
	int id; 					//id da operação
	int requesting_client;		//id do cliente que fez o pedido
	int requested_enterp;			//id da empresa pretendida
		
	char status;				//estado da operação
	int receiving_client;		//id do cliente que recebeu pedido
	int receiving_interm;		//id do intermediário que fez entrega
	int receiving_enterp;		//id do empresa que recebeu a encomenda

	struct timespec start_time; //quando o pedido foi criado
	struct timespec client_time; //quando o cliente recebeu //o pedido
	struct timespec intermed_time; //quando o intermediário recebeu //o pedido
	struct timespec enterp_time; //quando a empresa recebeu //o pedido
};


//estrutura que agrega os shared memory buffers necessários para comunicação entre processos
struct comm_buffers {
	struct rnd_access_buffer* main_client; 		//buffer para main enviar pedidos a clientes
	struct circular_buffer* client_interm;	//buffer para clientes encaminharem pedidos a intermediários
	struct rnd_access_buffer* interm_enterp;  	//buffer para intermediários entregarem pedidos a empresas
};


/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e 
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size);


/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um 
* apontador para a mesma.
*/
void* create_dynamic_memory(int size);


/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size);


/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr);


/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os clientes. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);


/* Função que escreve uma operação no buffer de memória partilhada entre os clientes
* e intermediários. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);


/* Função que escreve uma operação no buffer de memória partilhada entre os intermediários
* e as empresas. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_interm_enterp_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os clientes, se houver alguma disponível para ler que seja direcionada ao cliente especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre os clientes e intermediários,
* se houver alguma disponível para ler (qualquer intermediário pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre os intermediários e as empresas,
* se houver alguma disponível para ler dirijida à empresa especificada. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_interm_enterp_buffer(struct rnd_access_buffer* buffer, int enterp_id, int buffer_size, struct operation* op);


#endif

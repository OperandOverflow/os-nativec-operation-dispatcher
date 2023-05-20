/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#ifndef MAIN_PRIVATE_H_GUARD
#define MAIN_PRIVATE_H_GUARD

#include "main.h"
#include "synchronization.h"

// Função que converte um estado para um número que tenha ordem.
// ex: convert_status_to_int('M') < convert_status_to_int('C') < convert_status_to_int('I')
// ex: convert_status_to_int('I') < convert_status_to_int('A') < convert_status_to_int('E')
int convert_status_to_int(char status);

// Função que imprime o menu de usage (se aplicavel)
void usage_menu(int argc, char** argv);

// Função para limpar buffer de input do utilizador
void flush();

// Função que liberta memória alocada ao programa durante a sua execução
void cleanup_memory(struct comm_buffers* buffers, struct main_data* data);

// Função que imprime as estatisticas de n entidades de acordo com as estatisticas da mesma
void write_statistic(char* entity, int n, int* stats);

// Função que imprime o menu de ajuda
void help();

// type LaunchFunc representa um ponteiro para uma funcao de launch de processos
typedef int (*LaunchFunc)(int, struct comm_buffers*, struct main_data*, struct semaphores* sems);

/* Função que verifica se a condicao condition se verifica e,
se for o caso, lanca uma excecao com a mensagem error_msg. 
Indica na excecao o "snippet_id" associado ao erro.
*/
void verify_condition(int condition, char* snippet_id, char* error_msg, int status);

/* Funcao que lanca um processo, dado os buffers de comunicacao, 
os dados da main, a lista de pids a ser alterada, o numero de processos
a serem lancados e uma funcao para lancar esses processos.
*/
void launch_process(struct comm_buffers* buffers, struct main_data* data, int* pids, int n, LaunchFunc launch_func, struct semaphores* sem);

/* Funcao que trata do sinal SIGINT, termina a execucao dos processos
filhos, liberta os recursos e escreve as estatisticas.
*/
void signal_handler_main(int i);

/* Funcao que trata do sinal SIGALRM, imprime o estado dos pedidos
e volta a por o temporizador
*/
void alarm_handler(int interval);

void alarm_print_status(struct main_data* data, struct semaphores* sems);

// Program arguments-related constants
#define NUMBER_OF_ARGS 2
#define USAGE_STR   "\033[1mUsage:\033[0m \033[33m./AdmPor\033[0m \033[32m./config.txt\033[0m\n"\
                    "\033[1mOptions:\033[0m\n"\
                    "  \033[32m-h\033[0m: Print this usage message\n"


// User interation constants
#define ADMPOR_SHELL "\033[1;32madmpor:~/\033[0m$ \033[?12;25h"

// Information messages
//     - Create request
#define INFO_CREATED_OP "\033[1;32m[+]\033[0m \033[1;36m%s <%d>\033[0m was successfully created!\033[0m\n"
//     - Received request
#define INFO_RECEIVED_OP "\033[1;32m[+]\033[0m \033[1;36m%s <%d>\033[0m received \033[1;34moperation request <%d>\033[0m\n"
//     - Request status
#define INFO_STATUS_OPERATION           "\033[0;33m[?]\033[0m \033[1;34mOperation <%d>\033[0m status %c\n"
#define INFO_STATUS_MAIN_CREATED        "    -> \033[1;32mMain\033[0m created operation: requested by Client <%d>, targeting Enterprise <%d>\n"
#define INFO_STATUS_CLIENT_PROCESSED    "    -> \033[1;36mClient <%d>\033[0m processed operation\n"
#define INFO_STATUS_INTERM_PROCESSED    "    -> \033[1;36mIntermediary <%d>\033[0m processed operation\n"
#define INFO_STATUS_ENTERP_BOOKED       "    -> \033[1;36mEnterprise <%d>\033[0m booked operation\n"
#define INFO_STATUS_ENTERP_PROCESSED    "    -> \033[1;36mEnterprise <%d>\033[0m processed operation\n"
//     - Routine request status update
#define ALARM_MSG_BEGIN                 "\n[T] Routine operation status update\n"
#define ALARM_MSG_NOP                   "    # No operation has been created\n"
#define ALARM_MSG_OPERATION             "    * \033[1;34mOperation <%d>\033[0m status %c\n"
#define ALARM_MSG_MAIN_CREATED          "       -> \033[1;32mMain\033[0m created operation at %lld\n"
#define ALARM_MSG_CLIENT_PROCESSED      "       -> \033[1;36mClient <%d>\033[0m processed operation at %lld\n"
#define ALARM_MSG_INTERM_PROCESSED      "       -> \033[1;36mIntermediary <%d>\033[0m processed operation at %lld\n"
#define ALARM_MSG_ENTERP_BOOKED         "       -> \033[1;36mEnterprise <%d>\033[0m booked operation at %lld\n\n"
#define ALAMR_MSG_ENTERP_PROCESSED      "       -> \033[1;36mEnterprise <%d>\033[0m processed operation at %lld\n\n"


// Error handling constants
#define ERROR_ARGS "\033[0;31m[!] Error:\033[0m Number of arguments is should be 5. Execute `AdmPor -h` for 'help'.\n"
#define ERROR_MALLOC "\033[0;31m[!] Error:\033[0m Failed to allocate memory.\n"
#define ERROR_CLIENT_ID "\033[0;31m[!] Error:\033[0m The referred client doesn't exist!\n"
#define ERROR_ENTERPRISE_ID "\033[0;31m[!] Error:\033[0m The referred enterprise doesn't exist!\n"
#define ERROR_OPERATION_ID "\033[0;31m[!] Error:\033[0m The referred operation doesn't exist!\n"
#define ERROR_UNRECOGNIZED_COMMAND "\033[0;31m[!] Error:\033[0m Unrecognized command! Select option `help` for help.\n"
#define ERROR_IDS_OUT_OF_BOUNDS "\033[0;31m[!] Error:\033[0m Unable to create request, since provided IDs are not valid.\n"
#define ERROR_INVALID_INPUT "\033[0;31m[!] Error:\033[0m Invalid input.\n" 

// Sections
#define INIT_DS "Init DS"
#define INIT_COMM_BUFFER "Init comm buffer"
#define INIT_MAIN "Init main Args"
#define INIT_DMEM_BUFFERS "Init dmem buffers"
#define INIT_SHMEM_BUFFERS "Init shmem buffers"

// Booleans
#define TRUE 1
#define FALSE 0

// Options, menus and help message
#define HELP_MSG "========================================= Help =========================================\n"\
                 " 1. op (client id) (enterprise id)   Create an operation from a client to an enterprise\n"\
                 " 2. status (operation id)            Check the status of the request\n"\
                 " 3. stop                             Stop the program and prints the statistics\n"\
                 " 4. help                             Access help menu\n"
#define MENU_MSG "======================= Menu =======================\n"\
                 " op (client id) (enterprise id)\n"\
                 " status (operation id)\n"\
                 " stop\n"\
                 " help\n"\
                 "\n"

#define STATISTIC_MSG   "   ----------------------------------------------\n"\
                        "   |  %s ID   |     Processed operations  |\n"\
                        "   ----------------------------------------------\n"\
                        "%s"\
                        "   ----------------------------------------------\n"

#endif
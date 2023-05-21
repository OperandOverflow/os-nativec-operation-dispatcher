/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "main.h"
#include "main-private.h"
#include "memory.h"
#include "memory-private.h"
#include "aptime.h"
#include "process.h"
#include "process-private.h"
#include "apsignal.h"
#include "configuration.h"
#include "synchronization.h"
#include "synchronization-private.h"
#include "log.h"
#include "stats.h"


#ifndef ADMPOR_GLOBAL_VARIABLES
// ====================================================================================================
//                                        Global Variables
// ====================================================================================================
struct AdmPorData admpor; // global AdmPor struct
int operation_number = 0; // global counter of created operations
#endif

#ifndef ADMPOR_PROGRAM_DATA_STRUCT
// ====================================================================================================
//                                    Program Data Struct (AdmPorData)
// ====================================================================================================
void ADMPORDATA_INIT(int argc, char* argv[]) {
    // init valid as FALSE and logger as NULL
    admpor.valid = FALSE;
    admpor.logger = NULL;
    int failed;

    //init data structures
    admpor.data = create_dynamic_memory(sizeof(struct main_data));
    admpor.buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    admpor.sems = create_dynamic_memory(sizeof(struct semaphores));
    failed = !admpor.data || !admpor.buffers || !admpor.sems;
    if (assert_error(failed, INIT_DS , ERROR_MALLOC))
        return;

    struct main_data* data = admpor.data;
    struct comm_buffers* buffers = admpor.buffers;
    struct semaphores* sems = admpor.sems;

    // init comm_buffers
    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    failed = !buffers->main_client || !buffers->client_interm || !buffers->interm_enterp;
    if (assert_error(failed, INIT_COMM_BUFFER , ERROR_MALLOC))
        return;

    // init semaphores
    sems->main_client = create_dynamic_memory(sizeof(struct prodcons));
    sems->client_interm = create_dynamic_memory(sizeof(struct prodcons));
    sems->interm_enterp = create_dynamic_memory(sizeof(struct prodcons));
    failed = !sems->main_client || !sems->client_interm || !sems->interm_enterp;
    if (assert_error(failed, INIT_COMM_BUFFER , ERROR_MALLOC))
        return;

    // parse config file
    main_args(argc, argv, data);
    if (data->buffers_size < 0)
        return; // verify if config parsing failed

    // init main data structure
    create_dynamic_memory_buffers(data);
    failed = !data->client_pids || !data->intermediary_pids || !data->enterprise_pids 
        || !data->client_stats || !data->intermediary_stats || !data->enterprise_stats;
    if (assert_error(failed, INIT_DMEM_BUFFERS, ERROR_MALLOC))
        return;

    // init shared memory buffers
    create_shared_memory_buffers(data, buffers);
    failed = !data->results || !data->terminate 
        || !buffers->main_client->ptrs || !buffers->main_client->buffer
        || !buffers->client_interm->ptrs || !buffers->client_interm->buffer
        || !buffers->interm_enterp->ptrs || !buffers->interm_enterp->buffer;
    if (assert_error(failed, INIT_SHMEM_BUFFERS, ERROR_MALLOC))
        return;

    // create semaphores 
    create_semaphores(data, sems);
    if (assert_error(!are_semaphores_valid(sems), INIT_SEMAPHORES, ERROR_SEM_CREATE))
        return;

    // admpor was successfully initialized!
    admpor.valid = TRUE;
    return;

}

void ADMPOR_EXIT(int status) {
    ADMPORDATA_FREE();
    exit(status);
}

void ADMPORDATA_FREE() {
    destroy_memory_buffers(admpor.data, admpor.buffers);
    destroy_semaphores(admpor.sems);
    if (admpor.logger) LOG_FREE(admpor.logger);
}
#endif

#ifndef ADMPOR_ERROR_HANDLING
// ====================================================================================================
//                                        Error Handling
// ====================================================================================================
int assert_error(int condition, char* snippet_id, char* error_msg) {
    if (condition)
        fprintf(stderr, "[%s] %s", snippet_id, error_msg);
    return condition;
}

void verify_condition(int condition, char* snippet_id, char* error_msg, int status) {
    if(assert_error(
        condition, snippet_id, error_msg
    )) exit(status);
}
#endif

#ifndef ADMPOR_CREATE_MEMORY
// ====================================================================================================
//                                          Create memory
// ====================================================================================================
void create_dynamic_memory_buffers(struct main_data *data) {
    data->client_pids = create_dynamic_memory(sizeof(uid_t) * data->n_clients);
    data->intermediary_pids = create_dynamic_memory(sizeof(uid_t) * data->n_intermediaries);
    data->enterprise_pids = create_dynamic_memory(sizeof(uid_t) * data->n_enterprises);
    data->client_stats = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->intermediary_stats = create_dynamic_memory(sizeof(int) * data->n_intermediaries);
    data->enterprise_stats = create_dynamic_memory(sizeof(int) * data->n_enterprises);
}

void create_shared_memory_buffers(struct main_data *data, struct comm_buffers *buffers) {
    // comm buffers
    buffers->main_client->ptrs = create_shared_memory(
        STR_SHM_MAIN_CLIENT_PTR, 
        data->buffers_size * sizeof(int)
    );

    buffers->main_client->buffer = create_shared_memory(
        STR_SHM_MAIN_CLIENT_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    buffers->client_interm->ptrs = create_shared_memory(
        STR_SHM_CLIENT_INTERM_PTR, 
        sizeof(struct pointers)
    );

    buffers->client_interm->buffer = create_shared_memory(
        STR_SHM_CLIENT_INTERM_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    buffers->interm_enterp->ptrs = create_shared_memory(
        STR_SHM_INTERM_ENTERP_PTR, 
        data->buffers_size * sizeof(int)
    );

    buffers->interm_enterp->buffer = create_shared_memory(
        STR_SHM_INTERM_ENTERP_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    // main_data-related fields
    data->results = create_shared_memory(
        STR_SHM_RESULTS, 
        sizeof(struct operation) * MAX_RESULTS
    ); 
    data->terminate = create_shared_memory(
        STR_SHM_TERMINATE, 
        sizeof(int)
    );
}

void create_semaphores(struct main_data* data, struct semaphores* sems) {
    sems->client_interm->full = semaphore_create(STR_SEM_CLIENT_INTERM_FULL, 0);
    sems->interm_enterp->full = semaphore_create(STR_SEM_INTERM_ENTERP_FULL, 0);
    sems->main_client->full = semaphore_create(STR_SEM_MAIN_CLIENT_FULL, 0);

    sems->client_interm->empty = semaphore_create(STR_SEM_CLIENT_INTERM_EMPTY, data->buffers_size);
    sems->interm_enterp->empty = semaphore_create(STR_SEM_INTERM_ENTERP_EMPTY, data->buffers_size);
    sems->main_client->empty = semaphore_create(STR_SEM_MAIN_CLIENT_EMPTY, data->buffers_size);

    sems->client_interm->mutex = semaphore_create(STR_SEM_CLIENT_INTERM_MUTEX, 1);
    sems->interm_enterp->mutex = semaphore_create(STR_SEM_INTERM_ENTERP_MUTEX, 1);
    sems->main_client->mutex = semaphore_create(STR_SEM_MAIN_CLIENT_MUTEX, 1);

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

#endif

#ifndef ADMPOR_DESTROY_MEMORY
// ====================================================================================================
//                                           Destroy memory
// ====================================================================================================
void destroy_dynamic_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    // destroy main_data
    main_data_dynamic_memory_free(data);
    destroy_dynamic_memory(data);

    // destroy comm buffers
    comm_buffers_dynamic_memory_free(buffers);
    destroy_dynamic_memory(buffers); 
}

void destroy_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    // first shared memory
    if (!data)
        return;

    if (buffers) {
        if (buffers->main_client) {
            destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, data->buffers_size);
            destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, data->buffers_size);
        }
        if (buffers->client_interm) {
            destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, data->buffers_size);
            destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, data->buffers_size);
        }
        if (buffers->interm_enterp) {
            destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, data->buffers_size);
            destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, data->buffers_size);
        }

    }

    destroy_shared_memory(STR_SHM_RESULTS, data->results, data->buffers_size);
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, data->buffers_size);

    // then dynamic memory
    destroy_dynamic_memory_buffers(data, buffers);  
}

void destroy_semaphores(struct semaphores* sems) {
    semaphore_destroy(STR_SEM_CLIENT_INTERM_FULL, sems->client_interm->full);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_FULL, sems->interm_enterp->full);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_FULL, sems->main_client->full);

    semaphore_destroy(STR_SEM_CLIENT_INTERM_EMPTY, sems->client_interm->empty);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_EMPTY, sems->interm_enterp->empty);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_EMPTY, sems->main_client->empty);

    semaphore_destroy(STR_SEM_CLIENT_INTERM_MUTEX, sems->client_interm->mutex);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_MUTEX, sems->interm_enterp->mutex);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_MUTEX, sems->main_client->mutex); 
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex); 

    destroy_dynamic_memory(sems->client_interm);
    destroy_dynamic_memory(sems->interm_enterp);
    destroy_dynamic_memory(sems->main_client);

    destroy_dynamic_memory(sems);
}
#endif

#ifndef ADMPOR_PROCESSES
// ====================================================================================================
//                                          Processes
// ====================================================================================================
void launch_process(struct comm_buffers* buffers, struct main_data* data, int* pids, int n, LaunchFunc launch_func, struct semaphores* sems) {
    // for each process to launch, use 0 as "root id" and save its pid
    for (int i = 0; i < n; i++)
        pids[i] = launch_func(i, buffers, data, sems);
}

void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_clients; i++)
        data->client_stats[i] = wait_process(data->client_pids[i]);
    for (int i = 0; i < data->n_intermediaries; i++)
        data->intermediary_stats[i] = wait_process(data->intermediary_pids[i]);
    for (int i = 0; i < data->n_enterprises; i++)
        data->enterprise_stats[i] = wait_process(data->enterprise_pids[i]);
}

void launch_processes(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    launch_process(buffers, data, data->client_pids, data->n_clients, launch_client, sems);
    launch_process(buffers, data, data->intermediary_pids, data->n_intermediaries, launch_interm, sems);
    launch_process(buffers, data, data->enterprise_pids, data->n_enterprises, launch_enterp, sems);
}


void wakeup_processes(struct main_data* data, struct semaphores* sems) {
    for (int i = 0; i < data->n_clients; i++)
        produce_end(sems->main_client);
    for (int i = 0; i < data->n_intermediaries; i++)
        produce_end(sems->client_interm);
    for (int i = 0; i < data->n_enterprises; i++)
        produce_end(sems->interm_enterp);
}
#endif

#ifndef ADMPOR_SIGNALING
// ====================================================================================================
//                                           Signaling
// ====================================================================================================
void alarm_print_status(struct main_data* data, struct semaphores* sems) {
    semaphore_mutex_lock(sems->results_mutex);
    printf("\33[2K");
    printf(ALARM_MSG_BEGIN);
    if (operation_number == 0)
        printf(ALARM_MSG_NOP);
    for (int i = 0; i < operation_number; i++) {
        //Read operation status
        struct operation op = data->results[i];
        int statusInt = convert_status_to_int(op.status);
        printf(ALARM_MSG_OPERATION, op.id, op.status);
        printf(ALARM_MSG_MAIN_CREATED, convert_raw(&op.start_time));
        if (0 < statusInt)
            printf(ALARM_MSG_CLIENT_PROCESSED, op.receiving_client, convert_raw(&op.client_time));
        if (1 < statusInt)
            printf(ALARM_MSG_INTERM_PROCESSED, op.receiving_interm, convert_raw(&op.intermed_time));
        if (3 == statusInt)
            printf(ALARM_MSG_ENTERP_BOOKED, op.receiving_enterp, convert_raw(&op.enterp_time));
        if (3 < statusInt)
            printf(ALAMR_MSG_ENTERP_PROCESSED, op.receiving_enterp, convert_raw(&op.enterp_time));
    }
    printf(ADMPOR_SHELL);
    semaphore_mutex_unlock(sems->results_mutex);
}

void signal_handler_main(int i) {
    stop_execution(admpor.data, admpor.buffers, admpor.sems);
}

void alarm_handler(int interval) {
    alarm_print_status(admpor.data, admpor.sems);
    set_timer(admpor.data->alarm_time, alarm_handler);
}
#endif

#ifndef ADMPOR_PROGRAM_EXECUTION_OPTIONS
// ====================================================================================================
//                                      Program Execution Options
// ====================================================================================================
void usage_menu(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        // print usage string
        printf(USAGE_STR);
        // exit program
        exit(EXIT_SUCCESS);
    }
}

void main_args(int argc, char* argv[], struct main_data *data) { 
    parse_config_file(argv[1], data);
}
#endif


#ifndef ADMPOR_CLI
// ====================================================================================================
//                                      Command line Interface
// ====================================================================================================
void create_request(int* op_counter, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int client_id, enterprise_id;

    // read client id and enterprise id
    int read_items = scanf("%d %d", &client_id, &enterprise_id);

    char command[32];
    sprintf(command, "op %d %d", client_id, enterprise_id);
    ADMPOR_LOG(admpor.logger, command);

    if (read_items != 2) {
        printf(ERROR_INVALID_INPUT);
        flush();
        return;
    }

    if (client_id < 0 || client_id >= data->n_clients || enterprise_id < 0 || enterprise_id >= data->n_enterprises) {
        printf(ERROR_IDS_OUT_OF_BOUNDS);
        return;
    }

    struct operation op = {0, 0, 0, 0, 0};

    semaphore_mutex_lock(sems->results_mutex);
    data->results[*op_counter] = op;
    op.id = *op_counter;
    op.requesting_client = client_id;
    op.requested_enterp = enterprise_id;
    op.status = 'M';
    struct timespec process_time; // declare empty struct
    set_current_time(&process_time); // fill with current time
    op.start_time = process_time; // register the time of start
    semaphore_mutex_unlock(sems->results_mutex);

    printf(INFO_CREATED_OP, "Operation request", op.id);

    produce_begin(sems->main_client);
    write_main_client_buffer(buffers->main_client, data->buffers_size, &op);
    produce_end(sems->main_client);
    (*op_counter)++; 
}

int convert_status_to_int(char status) {
    //Associate each status with a number
    int statusInt = 0;
    if (status == 'C')
        statusInt = 1;
    else if (status == 'I')
        statusInt = 2;
    else if (status == 'A')
        statusInt = 3;
    else if (status == 'E')
        statusInt = 4;   
    return statusInt;
}

void read_status(struct main_data* data, struct semaphores* sems) {
    // read operation id
    int operation_id;
    int read_items = scanf("%d", &operation_id);

    char command[32];
    sprintf(command, "status %d", operation_id);
    ADMPOR_LOG(admpor.logger, command);

    if (read_items != 1) {
        printf(ERROR_INVALID_INPUT);
        flush();
        return;
    }

    //Validate input
    if (operation_id < 0 || operation_id >= operation_number) {
        printf(ERROR_OPERATION_ID);
        return;
    }

    //Read operation status
    semaphore_mutex_lock(sems->results_mutex);
    struct operation op = data->results[operation_id];
    semaphore_mutex_unlock(sems->results_mutex);
    int statusInt = convert_status_to_int(op.status);

    printf(INFO_STATUS_OPERATION, op.id, op.status);
    printf(INFO_STATUS_MAIN_CREATED, op.requesting_client, op.requested_enterp);
    if (0 < statusInt)
        printf(INFO_STATUS_CLIENT_PROCESSED, op.receiving_client);
    if (1 < statusInt)
        printf(INFO_STATUS_INTERM_PROCESSED, op.receiving_interm);
    if (3 == statusInt)
        printf(INFO_STATUS_ENTERP_BOOKED, op.receiving_enterp);
    if (3 < statusInt)
        printf(INFO_STATUS_ENTERP_PROCESSED, op.receiving_enterp);
}

void help() {
    printf("%s\n", HELP_MSG);
}

void write_statistic(char* entity, int n, int* stats) {
    char rows[1024] = "";

    for (int i = 0; i < n; i++) {
        // format the string and write it to a temporary buffer
        char temp[1024];
        snprintf(temp, 1024, "   | %-15d| %25d |\n", i, stats[i]);
        // append the temporary buffer to the main buffer
        strcat(rows, temp);
    }

    printf(STATISTIC_MSG, entity, rows);
}

void write_statistics(struct main_data* data) {
    printf("\n====================\033[0;32m Statistics\033[0m ====================\n");
    printf(" Max number of operations: %24d\n",     data->max_ops);
    printf(" Buffer size: %37d\n",                  data->buffers_size);

    printf("\n Number of clients: %31d\n",            data->n_clients);
    write_statistic("Client  ", data->n_clients, data->client_stats);
    
    printf("\n Number of intermediaries: %24d\n",     data->n_intermediaries);
    write_statistic("Interm. ", data->n_intermediaries, data->intermediary_stats);
    
    
    printf("\n Number of enterprises: %27d\n",        data->n_enterprises);
    write_statistic("Enterpr.", data->n_enterprises, data->enterprise_stats);

}

void stop_execution(struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems) {
    *(data->terminate) = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_stats(data, operation_number);
    ADMPOR_EXIT(EXIT_SUCCESS);
}

void user_interaction(struct comm_buffers *buffers, struct main_data *data, struct semaphores* sems) {
    char input[32]; // user input buffer

    while (*(data->terminate) == 0) {
        // sleep 300000 micro sec and launch shell 
        usleep(300000);
        printf(ADMPOR_SHELL);

        // ask for input
        if (scanf("%31s", input) != 1) {
            printf(ERROR_INVALID_INPUT);
            flush();
            return;
        }
        if (strcmp(input, "op") == 0) {
            create_request(&operation_number, buffers, data, sems);
        } else if (strcmp(input, "status") == 0) {
            read_status(data, sems);
        } else if (strcmp(input, "stop") == 0) {
            ADMPOR_LOG(admpor.logger, "stop");
            stop_execution(data, buffers, sems);
        } else if (strcmp(input, "help") == 0) {
            ADMPOR_LOG(admpor.logger, "help");
            help();
        } else {
            ADMPOR_LOG(admpor.logger, input);
            printf(ERROR_UNRECOGNIZED_COMMAND);
        }
    }
}
#endif

#ifndef ADMPOR_MAIN
// ====================================================================================================
//                                              Main
// ====================================================================================================
int main(int argc, char *argv[]) {
    // launch usage menu
    usage_menu(argc, argv);
    verify_condition(
        argc != NUMBER_OF_ARGS,
        INIT_MAIN,
        ERROR_ARGS,
        EXIT_FAILURE
    );

    // init AdmPor
    ADMPORDATA_INIT(argc, argv);
    if (!admpor.valid)
        ADMPOR_EXIT(EXIT_FAILURE);
    
    // launch clients, interms and enterps
    launch_processes(admpor.buffers, admpor.data, admpor.sems);

    // init logger
    admpor.logger = LOG_INIT(admpor.data->log_filename);
    if (!admpor.logger || !admpor.logger->ptr)
        ADMPOR_EXIT(EXIT_FAILURE);

    // associate SIGINT with a handler function
    set_intr_handler(signal_handler_main);

    set_timer(admpor.data->alarm_time, alarm_handler);

    printf(MENU_MSG);
    // launch user interaction menu
    user_interaction(admpor.buffers, admpor.data, admpor.sems);
}
#endif
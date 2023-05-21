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
#include "aptime.h"
#include "process.h"
#include "apsignal.h"
#include "configuration.h"
#include "synchronization.h"
#include "synchronization-private.h"
#include "log.h"
#include "stats.h"


// global logger
struct LoggingFile* logger = NULL;

// global counter of created operations
int operation_number = 0;

// global variables
struct main_data* global_data;
struct comm_buffers* global_buffers;
struct semaphores* global_sems;

void verify_condition(int condition, char* snippet_id, char* error_msg, int status) {
    if (condition) {
        fprintf(stderr, "[%s] %s", snippet_id, error_msg);
        exit(status);        
    }
}

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

void launch_process(struct comm_buffers* buffers, struct main_data* data, int* pids, int n, LaunchFunc launch_func, struct semaphores* sems) {
    // for each process to launch, use 0 as "root id" and save its pid
    for (int i = 0; i < n; i++)
        pids[i] = launch_func(i, buffers, data, sems);
}

void flush() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void create_request(int* op_counter, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int client_id, enterprise_id;

    // read client id and enterprise id
    int read_items = scanf("%d %d", &client_id, &enterprise_id);

    char command[32];
    sprintf(command, "op %d %d", client_id, enterprise_id);
    ADMPOR_LOG(logger, command);

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
    ADMPOR_LOG(logger, command);

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
    ADMPOR_LOG(logger, "help");
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

void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_clients; i++)
        data->client_stats[i] = wait_process(data->client_pids[i]);
    for (int i = 0; i < data->n_intermediaries; i++)
        data->intermediary_stats[i] = wait_process(data->intermediary_pids[i]);
    for (int i = 0; i < data->n_enterprises; i++)
        data->enterprise_stats[i] = wait_process(data->enterprise_pids[i]);
}

void destroy_dynamic_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->intermediary_pids);
    destroy_dynamic_memory(data->enterprise_pids);
    destroy_dynamic_memory(data->client_stats);
    destroy_dynamic_memory(data->intermediary_stats);
    destroy_dynamic_memory(data->enterprise_stats);
    destroy_dynamic_memory(data->log_filename);
    destroy_dynamic_memory(data->statistics_filename);
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers); 
}

void destroy_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    // first shared memory
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_RESULTS, data->results, data->buffers_size);
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, data->buffers_size);

    // then dynamic memory
    destroy_dynamic_memory_buffers(data, buffers);
   
}


void stop_execution(struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems) {
    *(data->terminate) = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_stats(data, operation_number);
    destroy_memory_buffers(data, buffers);
    destroy_semaphores(sems);
    LOG_FREE(logger);
}

void user_interaction(struct comm_buffers *buffers, struct main_data *data, struct semaphores* sems) {
    int running = TRUE;
    char input[32]; // user input buffer

    while (running) {
        // sleep 300000 micro sec and launch shell 
        usleep(300000);
        printf(ADMPOR_SHELL);

        // ask for input
        if (scanf("%s", &input) != 1) {
            printf(ERROR_INVALID_INPUT);
            flush();
            return;
        }
        if (strcmp(input, "op") == 0) {
            create_request(&operation_number, buffers, data, sems);
        } else if (strcmp(input, "status") == 0) {
            read_status(data, sems);
        } else if (strcmp(input, "stop") == 0) {
            ADMPOR_LOG(logger, "stop");
            running = FALSE;
            stop_execution(data, buffers, sems);
        } else if (strcmp(input, "help") == 0) {
            help();
        } else {
            ADMPOR_LOG(logger, input);
            printf(ERROR_UNRECOGNIZED_COMMAND);
        }
    }
}

void launch_processes(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    launch_process(buffers, data, data->client_pids, data->n_clients, launch_client, sems);
    launch_process(buffers, data, data->intermediary_pids, data->n_intermediaries, launch_interm, sems);
    launch_process(buffers, data, data->enterprise_pids, data->n_enterprises, launch_enterp, sems);
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

void wakeup_processes(struct main_data* data, struct semaphores* sems) {
    for (int i = 0; i < data->n_clients; i++)
        produce_end(sems->main_client);
    for (int i = 0; i < data->n_intermediaries; i++)
        produce_end(sems->client_interm);
    for (int i = 0; i < data->n_enterprises; i++)
        produce_end(sems->interm_enterp);
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

void alarm_print_status(struct main_data* data, struct semaphores* sems) {
    semaphore_mutex_lock(sems->results_mutex);
    printf("\33[2K");
    printf(ALARM_MSG_BEGIN);
    if (operation_number == 0)
        printf(ALARM_MSG_NOP);
    for (int i = 0; i < operation_number; i++)
    {
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
    // printf("\033^[[20h\n\033[0m");
    printf(ADMPOR_SHELL);
    semaphore_mutex_unlock(sems->results_mutex);
}

void signal_handler_main(int i) {
    stop_execution(global_data, global_buffers, global_sems);
    exit(EXIT_SUCCESS);
}

void alarm_handler(int interval) {
    alarm_print_status(global_data, global_sems);
    set_timer(global_data->alarm_time, alarm_handler);
}

int main(int argc, char *argv[]) {
    
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct comm_buffers* buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    struct semaphores* sems = create_dynamic_memory(sizeof(struct semaphores));
    verify_condition(!data || !buffers || !sems, INIT_DS , ERROR_MALLOC, EXIT_FAILURE);

    

    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    verify_condition(
        !buffers->main_client || !buffers->client_interm || !buffers->interm_enterp, 
        INIT_COMM_BUFFER,
        ERROR_MALLOC,
        EXIT_FAILURE
    );

    // init semaphore data structure
    sems->main_client = create_dynamic_memory(sizeof(struct prodcons));
    sems->client_interm = create_dynamic_memory(sizeof(struct prodcons));
    sems->interm_enterp = create_dynamic_memory(sizeof(struct prodcons));
    verify_condition(
        !sems->main_client || !sems->client_interm || !sems->interm_enterp, 
        INIT_COMM_BUFFER,
        ERROR_MALLOC,
        EXIT_FAILURE
    );

    // launch usage menu
    usage_menu(argc, argv);
    verify_condition(
        argc != NUMBER_OF_ARGS,
        INIT_MAIN,
        ERROR_ARGS,
        EXIT_FAILURE
    );
    //execute main code
    main_args(argc, argv, data);    
    create_dynamic_memory_buffers(data);
    verify_condition(
        !data->client_pids || !data->intermediary_pids || !data->enterprise_pids 
        || !data->client_stats || !data->intermediary_stats || !data->enterprise_stats, 
        INIT_DMEM_BUFFERS,
        ERROR_MALLOC,
        EXIT_FAILURE
    );
    
    create_shared_memory_buffers(data, buffers);
    verify_condition(
        !data->results || !data->terminate 
        || !buffers->main_client->ptrs || !buffers->main_client->buffer
        || !buffers->client_interm->ptrs || !buffers->client_interm->buffer
        || !buffers->interm_enterp->ptrs || !buffers->interm_enterp->buffer, 
        INIT_SHMEM_BUFFERS,
        ERROR_MALLOC,
        EXIT_FAILURE
    );

    // create semaphores 
    create_semaphores(data, sems);
    if (!are_semaphores_valid(sems)) {
        printf(ERROR_SEM_CREATE);
        destroy_semaphores(sems);
        exit(EXIT_SEM_CREATE_ERROR);
    }

    // launch clients, interms and enterps
    launch_processes(buffers, data, sems);

    // ==============================
    global_data = data;
    global_buffers = buffers;
    global_sems = sems;
    // ==============================

    // init logger
    logger = LOG_INIT(data->log_filename);

    // associate SIGINT with a handler function
    set_intr_handler(signal_handler_main);

    set_timer(data->alarm_time, alarm_handler);

    printf(MENU_MSG);
    // launch user interaction menu
    user_interaction(buffers, data, sems);
}
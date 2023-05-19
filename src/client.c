/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "client.h"
#include "memory.h"
#include "aptime.h"
#include "apsignal.h"
#include "main-private.h"
#include "synchronization.h"
#include "synchronization-private.h"
#include "client-private.h"

int op_counter_c;

int execute_client(int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // first, setup op and op_counter_c
    struct operation op = {0, 0, 0, 0, 0, 0};
    op_counter_c = 0;
    // then associate SIGINT with the handler function
    set_intr_handler(signal_handler_client);
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        client_get_operation(&op, client_id, buffers, data, sems); // read op from buffer
        // if there's no op to do
        if (op.id == -1)
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Client", client_id, op.id);
    
        // found operation to do! processing it...
        client_process_operation(&op, client_id, data, &op_counter_c, sems);
        client_send_operation(&op, buffers, data, sems);
    }
    // return counter of processed operations
    return op_counter_c;
}

void client_get_operation(struct operation* op, int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read main-client buffer
    consume_begin(sems->main_client);
    read_main_client_buffer(buffers->main_client, client_id, data->buffers_size, op);
    if (op->id == -1)
        // undo buffer consume
        consume_begin_undo(sems->main_client);
    else 
        // release read permission
        consume_end(sems->main_client);
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_client = client_id; // update receiving client field
    op->status = 'C'; // change status to 'C' (processed by client)
    struct timespec process_time; // declare empty struct
    getcurrenttime(&process_time); // fill with current time
    op->client_time = process_time; // register the time of process
    (*counter)++; // increment counter of this client's processed operations
    semaphore_mutex_lock(sems->results_mutex);
    memcpy(&data->results[op->id], op, sizeof(struct operation));
    semaphore_mutex_unlock(sems->results_mutex);
}

void client_send_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // send operation to interm buffer
    produce_begin(sems->client_interm);
    write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
    produce_end(sems->client_interm);
}

void signal_handler_client(int i) {
    exit(op_counter_c);
}
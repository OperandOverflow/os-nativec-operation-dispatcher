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

#include "main.h"
#include "memory.h"
#include "aptime.h"
#include "apsignal.h"
#include "intermediary.h"
#include "main-private.h"
#include "synchronization.h"

int execute_intermediary(int interm_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // first, setup op and op_counter
    struct operation op = {0, 0, 0, 0, 0, 0};
    int op_counter_i = 0;
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        intermediary_receive_operation(&op, buffers, data, sems); // read op from buffer
        // if there's no op to do
        if (op.id == -1) 
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Intermediary", interm_id, op.id);
        
        // found operation to do! processing it...
        intermediary_process_operation(&op, interm_id, data, &op_counter_i, sems);
        intermediary_send_answer(&op, buffers, data, sems);

    }
    // return counter of processed operations
    return op_counter_i;
}

void intermediary_receive_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read client-interm buffer
    consume_begin(sems->client_interm);
    read_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
    consume_end(sems->client_interm);
}

void intermediary_process_operation(struct operation* op, int interm_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_interm = interm_id; // update receiving interm field
    op->status = 'I'; // change status to 'I' (processed by interm)
    struct timespec process_time; // declare empty struct
    set_current_time(&process_time); // fill with current time
    op->intermed_time = process_time; // register the time of process
    (*counter)++; // increment counter of this interm's processed operations
    semaphore_mutex_lock(sems->results_mutex);
    memcpy(&data->results[op->id], op, sizeof(struct operation));
    semaphore_mutex_unlock(sems->results_mutex);
}

void intermediary_send_answer(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    produce_begin(sems->interm_enterp);
    write_interm_enterp_buffer(buffers->interm_enterp, data->buffers_size, op);
    produce_end(sems->interm_enterp);
}
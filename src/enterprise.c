/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "enterprise.h"
#include "main.h"
#include "memory.h"
#include "aptime.h"
#include "apsignal.h"
#include "main-private.h"
#include "synchronization.h"
#include "synchronization-private.h"

int execute_enterprise(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // first, setup op and op_counter_e
    struct operation op = {0, 0, 0, 0, 0, 0};
    int op_counter_e = 0;
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        enterprise_receive_operation(&op, enterp_id, buffers, data, sems); // read op from buffer
        // if there's no op to do
        if (op.id == -1) 
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Enterprise", enterp_id, op.id);
        
        // found operation to do! processing it...
        enterprise_process_operation(&op, enterp_id, data, &op_counter_e, sems);

    }
    // return counter of processed operations
    return op_counter_e;
}

void enterprise_receive_operation(struct operation* op, int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read interm-enterprise buffer
    consume_begin(sems->interm_enterp);
    read_interm_enterp_buffer(buffers->interm_enterp, enterp_id, data->buffers_size, op);
    if (op->id == -1)
        // undo buffer consume
        consume_begin_undo(sems->interm_enterp);
    else 
        // release read permission
        consume_end(sems->interm_enterp);
}

void enterprise_process_operation(struct operation* op, int enterp_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_enterp = enterp_id; // update receiving enterp field
    // change status to 'A' or 'E' (processed by enterp)
    op->status = (op->id < data->max_ops) ? 'E' : 'A' ; 
    struct timespec process_time; // declare empty struct
    set_current_time(&process_time); // fill with current time
    op->enterp_time = process_time; // register the time of process
    (*counter)++; // increment counter of this enterprise's processed operations
    semaphore_mutex_lock(sems->results_mutex);
    memcpy(&data->results[op->id], op, sizeof(struct operation));
    semaphore_mutex_unlock(sems->results_mutex);
}
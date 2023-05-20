/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "process.h"
#include "client.h"
#include "intermediary.h"
#include "enterprise.h"
#include "synchronization.h"
#include "main-private.h"
#include "process-private.h"

void admpor_child_process_free(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    destroy_semaphores(sems);
    destroy_dynamic_memory_buffers(data, buffers);
}

int launch_client(int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    pid_t pid = fork();
    if (pid == 0) { // child process
        int processed_operations = execute_client(client_id, buffers, data, sems);
        admpor_child_process_free(buffers, data, sems);
        exit(processed_operations);
    }
    return pid;
}

int launch_interm(int interm_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    pid_t pid = fork();
    if (pid == 0) { // child process
        int processed_operations = execute_intermediary(interm_id, buffers, data, sems);
        admpor_child_process_free(buffers, data, sems);
        exit(processed_operations);
    }
    return pid;    
}

int launch_enterp(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    pid_t pid = fork();
    if (pid == 0) { // child process
        int processed_operations = execute_enterprise(enterp_id, buffers, data, sems);
        admpor_child_process_free(buffers, data, sems);
        exit(processed_operations);
    }
    return pid;    
}

int wait_process(int process_id) {
    int proccess_return;
    // wait for process response
    if (waitpid(process_id, &proccess_return, 0) == -1) {
        printf(ERROR_PROCESS_WAITPID, process_id);
    }

    // return process response if terminated properly
    if (WIFEXITED(proccess_return)) {
        return WEXITSTATUS(proccess_return);
    } else {
        printf(ERROR_PROCESS_EXIT_NORMALLY, process_id);
    }
    return 0;
}
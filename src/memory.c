/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include "memory.h"
#include "memory-private.h"
#include "main-private.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

void main_data_dynamic_memory_free(struct main_data* data) {
    if (data != NULL) {
        destroy_dynamic_memory(data->client_pids);
        destroy_dynamic_memory(data->intermediary_pids);
        destroy_dynamic_memory(data->enterprise_pids);
        destroy_dynamic_memory(data->client_stats);
        destroy_dynamic_memory(data->intermediary_stats);
        destroy_dynamic_memory(data->enterprise_stats);
        destroy_dynamic_memory(data->log_filename);
        destroy_dynamic_memory(data->statistics_filename);       
    }
}

void comm_buffers_dynamic_memory_free(struct comm_buffers* buffers) {
    if (buffers != NULL) {
        destroy_dynamic_memory(buffers->main_client);
        destroy_dynamic_memory(buffers->client_interm);
        destroy_dynamic_memory(buffers->interm_enterp);
    }
}

void flush() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void safe_free(void* ptr) {
    if (ptr) free(ptr);
}

void* create_dynamic_memory(int size) {
    return calloc(1, size);
}

void destroy_dynamic_memory(void* ptr) {
    safe_free(ptr);
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (!ptr) return;
    
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);

    assert_error(
        munmap(ptr, size) == -1,
        name_uid,
        ERROR_SHM_UNMAP
    );

    assert_error(
        shm_unlink(name_uid) == -1,
        name_uid,
        ERROR_SHM_UNLINK
    );
}

void* create_shared_memory(char *name, int size) {
    uid_t uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);
 
    int fd = shm_open(name_uid, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); 
    if (assert_error(
        fd == -1, 
        name_uid, 
        ERROR_SHM_OPEN
    )) return NULL;

    if (assert_error(
        ftruncate(fd, size) == -1, 
        name, 
        ERROR_SHM_TRUNCATE
    )) return NULL;

    int *shmem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if (assert_error(
        shmem_ptr == MAP_FAILED, 
        name, 
        ERROR_SHM_MAP
    )) return NULL;

    return shmem_ptr;
}

void write_operation_to_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is free
        if (buffer->ptrs[i] == FREE_MEM) {
            // write the operation to the buffer and update pointer to 1 (used)
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = USED_MEM;
            return; // exit the function
        }
    }   
}

void write_main_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    write_operation_to_rnd_access_buffer(buffer, buffer_size, op);
}

void write_interm_enterp_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    write_operation_to_rnd_access_buffer(buffer, buffer_size, op);  
}

void read_main_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is in used
        if (buffer->ptrs[i] == USED_MEM) {
            if (buffer->buffer[i].requesting_client == client_id) {
                // point op to buffered op and update ith position to free
                *op = buffer->buffer[i];
                buffer->ptrs[i] = FREE_MEM;
                return; // exit the function
            }
        }
    }
    // no op available
    op->id = -1;
}

void read_interm_enterp_buffer(struct rnd_access_buffer* buffer, int enterp_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is in used
        if (buffer->ptrs[i] == USED_MEM) {
            if (buffer->buffer[i].requested_enterp == enterp_id) {
                // point op to buffered op and update ith position to free
                *op = buffer->buffer[i];
                buffer->ptrs[i] = FREE_MEM;
                return; // exit the function
            }
        }
    }
    // no op available
    op->id = -1;
}

void write_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    // increase index for next read
    int next_index = (buffer->ptrs->in + 1) % buffer_size;

    // return if buffer is full
    if (next_index == buffer->ptrs->out)
        return;

    // write the operation to the buffer at the next available index
    buffer->buffer[buffer->ptrs->in] = *op;

    // update the write pointer to the next available index
    buffer->ptrs->in = next_index;
}

void read_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    // return if the buffer is empty, setting op id to -1
    if (buffer->ptrs->in == buffer->ptrs->out) {
        op->id = -1;
        return;
    }
    // read the operation from the buffer
    *op = buffer->buffer[buffer->ptrs->out];
    // update the out pointer if reading was successful
    buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
}
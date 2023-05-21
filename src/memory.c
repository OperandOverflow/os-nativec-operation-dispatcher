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

    verify_condition(
        munmap(ptr, size) == -1,
        name,
        ERROR_SHM_UNMAP, 
        EXIT_MUNMAP_ERROR
    );

    verify_condition(
        shm_unlink(name) == -1,
        name,
        ERROR_SHM_UNLINK, 
        EXIT_SHM_UNLINK_ERROR
    );
}

void* create_shared_memory(char *name, int size) {
    uid_t uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);
 
    int fd = shm_open(name, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); 
    verify_condition(
        fd == -1, 
        name, 
        ERROR_SHM_OPEN,
        EXIT_SHM_OPEN_ERROR
    );

    verify_condition(
        ftruncate(fd, size) == -1, 
        name, 
        ERROR_SHM_TRUNCATE,
        EXIT_SHM_TRUNCATE_ERROR
    );

    int *shmem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    verify_condition(
        shmem_ptr == MAP_FAILED, 
        name, 
        ERROR_SHM_MAP,
        EXIT_SHM_MAP_ERROR
    );

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
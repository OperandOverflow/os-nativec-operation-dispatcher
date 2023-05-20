#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stats.h"
#include "aptime.h"
#include "main.h"
#include "main-private.h"
#include "memory.h"

FILE* fpointer;

void write_stats(struct main_data* data, int op_counter) {
    open_file(data->statistics_filename);
    write_content(data, op_counter);
    close_file();
}

void open_file(char* filename) {
    fpointer = fopen(filename, "w");
    verify_condition(
        fpointer == NULL, 
        INIT_STATS, 
        ERROR_FAILED_OPEN_FILE, 
        EXIT_OPEN_FILE_ERROR
    );
}

void write_content(struct main_data* data, int op_counter) {
    // begin message
    fprintf(fpointer, STATS_START);
    fflush(fpointer);

    // processed operations
    for (int i = 0; i < data->n_clients; i++)
        fprintf(fpointer, STATS_CLIENTS, i, data->client_stats[i]);
    for (int i = 0; i < data->n_intermediaries; i++)
        fprintf(fpointer, STATS_INTERM, i, data->intermediary_stats[i]);
    for (int i = 0; i < data->n_enterprises; i++)
        fprintf(fpointer, STATS_ENTERP, i, data->enterprise_stats[i]);
    fflush(fpointer);

    // begin requests
    fprintf(fpointer, STATS_REQUESTS);
    fflush(fpointer);
    for (int i = 0; i < op_counter; i++) {
        fprintf(fpointer, STATS_REQUEST, data->results[i].id, data->results[i].status);
        fprintf(
            fpointer, 
            STATS_REQUEST_IDS, 
            data->results[i].receiving_client, 
            data->results[i].receiving_interm, 
            data->results[i].receiving_enterp
        );
        int size = 24;

        char startT[size];
        convert_localtime(&(data->results[i].start_time) , startT, size);

        char clientT[size];
        convert_localtime(&(data->results[i].client_time) , clientT, size);

        char intermT[size];
        convert_localtime(&(data->results[i].intermed_time) , intermT, size);

        char enterpT[size];
        convert_localtime(&(data->results[i].enterp_time) , enterpT, size);

        char totalT[size];
        calculate_difference(&(data->results[i].enterp_time), &(data->results[i].start_time), totalT);

        fprintf(
            fpointer, 
            STATS_REQUEST_TIMES, 
            startT, 
            clientT, 
            intermT,
            enterpT,
            totalT
        );
    }
    fflush(fpointer);
}

void close_file() {
    fclose(fpointer);
}
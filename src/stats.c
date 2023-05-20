#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stats.h"
#include "aptime.h"
#include "main.h"
#include "main-private.h"
#include "memory.h"

struct StatsFile* STATS_INIT(char* filename) {
    struct StatsFile* stats = (struct StatsFile*)create_dynamic_memory(sizeof(struct StatsFile));
    if (stats != NULL) {
        stats->ptr = fopen(filename, "w");

        // verify if there was a problem during file opening
        verify_condition(
            stats->ptr == NULL,
            INIT_STATS,
            ERROR_FAILED_OPEN_STATSFILE,
            EXIT_OPEN_STATSFILE_ERROR
        );
        printf(INFO_LOADED_STATSFILE, filename);
    }
    return stats;
}

void STATS_FREE(struct StatsFile* stats) {
    fclose(stats->ptr);
    destroy_dynamic_memory(stats);
}


void write_stats(struct main_data* data, int op_counter) {
    struct StatsFile* stats = STATS_INIT(data->statistics_filename);
    write_content(data, op_counter, stats->ptr);
    STATS_FREE(stats);
}

void write_processed_operations(struct main_data* data, FILE* fpointer) {
    // processed operations
    for (int i = 0; i < data->n_clients; i++)
        fprintf(fpointer, STATS_CLIENTS, i, data->client_stats[i]);
    for (int i = 0; i < data->n_intermediaries; i++)
        fprintf(fpointer, STATS_INTERM, i, data->intermediary_stats[i]);
    for (int i = 0; i < data->n_enterprises; i++)
        fprintf(fpointer, STATS_ENTERP, i, data->enterprise_stats[i]);
    fflush(fpointer);
}


void write_operation_statistics(struct operation op, FILE* fpointer) {
    fprintf(fpointer, STATS_REQUEST, op.id, op.status);
    fprintf(
        fpointer, 
        STATS_REQUEST_IDS, 
        op.receiving_client, 
        op.receiving_interm, 
        op.receiving_enterp
    );
    int size = 24;

    char startT[size];
    convert_localtime(&(op.start_time) , startT, size);

    char clientT[size];
    convert_localtime(&(op.client_time) , clientT, size);

    char intermT[size];
    convert_localtime(&(op.intermed_time) , intermT, size);

    char enterpT[size];
    convert_localtime(&(op.enterp_time) , enterpT, size);

    char totalT[size];
    calculate_difference(&(op.enterp_time), &(op.start_time), totalT);

    fprintf(
        fpointer, 
        STATS_REQUEST_TIMES, 
        startT, 
        clientT, 
        intermT,
        enterpT,
        totalT
    );
    fflush(fpointer);

}

void write_content(struct main_data* data, int op_counter, FILE* fpointer) {
    // begin message
    fprintf(fpointer, STATS_START);
    fflush(fpointer);

    // begin requests
    fprintf(fpointer, STATS_REQUESTS);
    fflush(fpointer);
    for (int i = 0; i < op_counter; i++)
        write_operation_statistics(data->results[i], fpointer);
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stats.h"
#include "aptime.h"
#include "main.h"
#include "main-private.h"

FILE* fpointer;

void write_stats(struct main_data* data, int op_counter) {
    open_file(data->statistics_filename);
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

void close_file() {
    fclose(fpointer);
}
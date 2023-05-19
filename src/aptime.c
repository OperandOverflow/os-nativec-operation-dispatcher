#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aptime.h"


void getcurrenttime(struct timespec* spec) {
    if ( clock_gettime(CLOCK_REALTIME, spec) == -1)
    {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }
    return;
}

long long convert_raw(struct timespec* spec) {
    return spec->tv_sec;
}

void convert_localtime(struct timespec* spec, char* str, int size) {

    time_t rawtime = spec->tv_sec;
    struct tm* local;

    local = localtime(&rawtime);
    
    char miliseconds[16];
    int nanoseconds = ((int)(spec->tv_nsec))/1000000;
    sprintf(miliseconds, "%d", nanoseconds);

    strftime(str, size, "%F %H:%M:%S.", local);
    strcat(str, miliseconds);

}
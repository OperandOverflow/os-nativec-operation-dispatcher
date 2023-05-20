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

char* getCurrentTimeStr(char* format) {
    time_t rawtime;
    struct tm *local;
    static char datetime[20];

    time(&rawtime);
    local = localtime(&rawtime);
    strftime(datetime, sizeof(datetime), format, local);
    return datetime;
}

void calculate_difference(struct timespec* t1, struct timespec* t2, char* str) {
    if (t1->tv_sec < t2->tv_sec)
    {
        calculate_difference(t2, t1, str);
        return;
    }

    long int dif_sec = (long) difftime(t1->tv_sec, t2->tv_sec); // t1 - t2

    long long t1_nsec = t1->tv_nsec;
    long long t2_nsec = t2->tv_nsec;
    long long dif_nsec;
    if (t1_nsec < t2_nsec)
    {
        t1_nsec += 1000000000;
        dif_sec--;
    } 
    dif_nsec = t1_nsec - t2_nsec;
    dif_nsec /= 1000000;

    char nsec[24];

    sprintf(str, "%ld.", dif_sec);

    sprintf(nsec, "%d", dif_nsec);

    strcat(str, nsec);
}
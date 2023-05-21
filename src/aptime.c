#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aptime.h"
#include "main-private.h"


void set_current_time(struct timespec* spec) {
    assert_error(
        clock_gettime(CLOCK_REALTIME, spec) == -1,
        INIT_CLOCKGETTIME,
        ERROR_CLOCKGETTIME
    );
}

char* get_datetime_string_from_rawtime(time_t rawtime, char* format) {
    struct tm *local = localtime(&rawtime);
    static char datetime[32];

    strftime(datetime, sizeof(datetime), format, local);
    return datetime;
}

long long convert_raw_nsec(struct timespec* spec) {
    return spec->tv_sec * 1000000000LL + spec->tv_nsec;
}

long long convert_raw_sec(struct timespec* spec) {
    return convert_raw_nsec(spec) / 1000000000LL;
}

char* get_datetime_string_from_spec(struct timespec* spec) {
    // get string datetime from tv_sec
    char* datetime = get_datetime_string_from_rawtime(spec->tv_sec, "%Y-%m-%d %H:%M:%S");
    
    // compute decimal seconds (.values) from tv_nsec
    char miliseconds[16];
    sprintf(miliseconds, ".%.3ld", spec->tv_nsec / 1000000);

    // concat millisec to datetime
    strcat(datetime, miliseconds);
    return datetime;
}

char* get_current_datetime_string() {
    struct timespec spec;
    set_current_time(&spec);
    return get_datetime_string_from_spec(&spec);
}

void calculate_difference(struct timespec* t1, struct timespec* t2, char* str) {
    long long delta = convert_raw_nsec(t1) - convert_raw_nsec(t2);
    if (delta < 0)
        delta *= -1;

    int delta_sec = delta / 1000000000LL;
    int delta_msec = (delta % 1000000000LL) / 1000000;
    sprintf(str, "%d.%03d", delta_sec, delta_msec);
}
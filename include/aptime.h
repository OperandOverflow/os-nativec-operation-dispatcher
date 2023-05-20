#ifndef APTIME_H_GUARD
#define APTIME_H_GUARD

#include <time.h>

/**
 * Function that inicializes the refered buffer with a timespec struct
 * which contains the time when this function is called
 * @param spec      pointer to the buffer where the struct will be saved
*/
void getcurrenttime(struct timespec* spec);

/**
 * Function which receives a timespec struct and converts the contained
 * time to the raw format
 * @param spec      timespec that contains the time to be converted
 * @return          time in raw format
*/
long long convert_raw(struct timespec* spec);


/**
 * Function that receives a timespec struct and converts to a string
 * of the type yyyy-mm-dd hh:mm:ss:ms
 * @param spec      pointer to timespec that contains the time to be converted
 * @param str       pointer to the buffer where string will be saved
 * @param size      size of the buffer
*/
void convert_localtime(struct timespec* spec, char* str, int size);

/**
 * Function that receives a string format and returns the current time
 * in that specific format
 * @param format    format string
 * @return          pointer to the result string
*/
char* getCurrentTimeStr(char* format);

/**
 * Function that calculates the difference of time between two times(in seconds)
 * and converts it to string and saves it in the refered buffer
 * @param t1        pointer to timespec of the first time
 * @param t2        pointer to timespec of the second time
 * @param str       pointer to the buffer where string will be saved
*/
void calculate_difference(struct timespec* t1, struct timespec* t2, char* str);

#endif
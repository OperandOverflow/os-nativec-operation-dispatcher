#ifndef APTIME_H_GUARD
#define APTIME_H_GUARD

#include <time.h>

/**
 * Function that initializes the refered buffer with a timespec struct
 * which contains the time when this function is called
 * @param spec      pointer to the buffer where the struct will be saved
*/
void set_current_time(struct timespec* spec);

/**
 * Function which receives a timespec struct and converts the contained
 * time to the raw format (nano seconds)
 * @param spec      timespec that contains the time to be converted
 * @return          time in raw format
*/
long long convert_raw(struct timespec* spec);

/**
 * Function that receives a rawtime and converts to a string
 * of the given format
 * @param rawtime   time in raw format
 * @param format    formatting to use
 * @return          pointer to the result string
*/
char* get_datetime_string_from_rawtime(time_t rawtime, char* format);

/**
 * Function that receives a timespec struct and converts to a string
 * of the type yyyy-mm-dd hh:mm:ss:ms
 * @param spec      pointer to timespec that contains the time to be converted
 * @return          pointer to the result string
*/
char* get_datetime_string_from_spec(struct timespec* spec);

/**
 * Function that returns a string representation of the  
 * current time in the complete format (yyyy-mm-dd hh:mm:ss:ms)
 * @return          pointer to the result string
*/
char* get_current_datetime_string();

/**
 * Function that calculates the difference of time between two times(in seconds)
 * and converts it to string and saves it in the refered buffer
 * @param t1        pointer to timespec of the first time
 * @param t2        pointer to timespec of the second time
 * @param str       pointer to the buffer where string will be saved
*/
void calculate_difference(struct timespec* t1, struct timespec* t2, char* str);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_CLOCKGETTIME "Set Current Time"

#define ERROR_CLOCKGETTIME "Error: Failed to get current time using clock_gettime."

#define EXIT_CLOCKGETTIME_ERROR 601

#endif
#ifndef APSIGNAL_H_GUARD
#define APSIGNAL_H_GUARD

#include <signal.h>

/**
 * Function that constantly calls the function passed as 
 * parameter in the defined interval
 * @param interval  interval (in seconds) to call
 * @param f         funcion to be called
*/
void set_timer(int inter, void (*f)(int));

/**
 * Function that handles SIGINT signal and calls the 
 * function passed as parameter
 * @param f         function to be called when receives SIGINT
*/
void set_intr_handler(void (*handler)(int));

/**
 * Handler that ignores a signum signal
 * @param signum         signal to ignore
*/
void ignore_signal_handler(int signum);

/**
 * Function that setup a signal handler that ignores a signum signal
 * @param signum         signal to ignore
*/
void ignore_signal(int signum);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_ALARM "Init alarm signal"
#define INIT_INTR "Init interrupt signal"
#define INIT_IGNORE "Init ignore signal"

#define ERROR_REGISTER_SIGNAL_HANDLER "Error: Failed to register signal handler.\n"
#endif
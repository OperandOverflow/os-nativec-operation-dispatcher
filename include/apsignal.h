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
 * Function that sends a SIGINT signal to the refered process
 * @param process_id    process to which SIGINT will be sent
*/
void interrupt_process(int process_id);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_ALARM "Init alarm"
#define INIT_INTR "Init interrupt"

#define ERROR_REGISTER_SIGNAL_HANDLER "Error: Failed to register signal handler.\n"


#define EXIT_REGISTER_SIGNAL_HANDLER_ERROR 314
#endif
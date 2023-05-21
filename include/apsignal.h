#ifndef APSIGNAL_H_GUARD
#define APSIGNAL_H_GUARD

#include <signal.h>

/** Função que define a execução da função f a cada inter segundos */
void set_timer(int inter, void (*f)(int));

/* Função que define um controlador de SIGINT como a função handler */
void set_intr_handler(void (*handler)(int));

/* Função que ignora um sinal */
void ignore_signal_handler(int signum);

/* Função que define um controlador de signum como a função que ignora o sinal */
void ignore_signal(int signum);

// ====================================================================================================
//                                          ERROR HANDLING
// ====================================================================================================
// Sections
#define INIT_ALARM "Init alarm signal"
#define INIT_INTR "Init interrupt signal"
#define INIT_IGNORE "Init ignore signal"

// Error messages
#define ERROR_REGISTER_SIGNAL_HANDLER "Error: Failed to register signal handler.\n"
#endif
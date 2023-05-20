
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "apsignal.h"
#include "main-private.h"

void set_timer(int inter, void (*handler)(int)) {
    verify_condition(
        signal(SIGALRM, handler) == SIG_ERR,
        INIT_ALARM,
        ERROR_REGISTER_SIGNAL_HANDLER,
        EXIT_REGISTER_SIGNAL_HANDLER_ERROR
    );
    alarm(inter);
}

void set_intr_handler(void (*handler)(int)) {
    verify_condition(
        signal(SIGINT, handler) == SIG_ERR,
        INIT_INTR,
        ERROR_REGISTER_SIGNAL_HANDLER,
        EXIT_REGISTER_SIGNAL_HANDLER_ERROR
    );
}

void interrupt_process(int process_id) {
    kill(process_id, SIGINT);
}
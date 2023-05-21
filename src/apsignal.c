
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "apsignal.h"
#include "main-private.h"

void ignore_signal_handler(int signum) {
    return;
}

void ignore_signal(int signum) {
    signal(signum, ignore_signal_handler);
}

void set_timer(int inter, void (*handler)(int)) {
    // attempt to implement with sigaction
    struct sigaction sa;
    sa.sa_handler = handler; // set handler function
    sigemptyset(&sa.sa_mask); // no signal is blocked during execution of handler
    sa.sa_flags = SA_RESTART; // after handling, restart waiting processes
    assert_error(
        sigaction(SIGALRM, &sa, NULL) == SIG_ERR,
        INIT_ALARM,
        ERROR_REGISTER_SIGNAL_HANDLER
    );

    // traditional implementation
    // assert_error(
    //     signal(SIGALRM, handler) == SIG_ERR,
    //     INIT_ALARM,
    //     ERROR_REGISTER_SIGNAL_HANDLER
    // );
    alarm(inter);
}

void set_intr_handler(void (*handler)(int)) {
    assert_error(
        signal(SIGINT, handler) == SIG_ERR,
        INIT_INTR,
        ERROR_REGISTER_SIGNAL_HANDLER
    );
}
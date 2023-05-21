/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

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
    assert_error(
        signal(signum, ignore_signal_handler) == SIG_ERR,
        INIT_IGNORE,
        ERROR_REGISTER_SIGNAL_HANDLER
    );
}

void set_timer(int inter, void (*handler)(int)) {
    // attempt to implement with sigaction
    struct sigaction sa;
    sa.sa_handler = handler; // set handler function
    sigemptyset(&sa.sa_mask); // no signal is blocked during execution of handler
    sa.sa_flags = 0; // no flags
    assert_error(
        sigaction(SIGALRM, &sa, NULL) == -1,
        INIT_ALARM,
        ERROR_REGISTER_SIGNAL_HANDLER
    );
    alarm(inter);
}

void set_intr_handler(void (*handler)(int)) {
    assert_error(
        signal(SIGINT, handler) == SIG_ERR,
        INIT_INTR,
        ERROR_REGISTER_SIGNAL_HANDLER
    );
}
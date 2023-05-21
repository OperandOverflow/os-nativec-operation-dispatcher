
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
    assert_error(
        signal(SIGALRM, handler) == SIG_ERR,
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
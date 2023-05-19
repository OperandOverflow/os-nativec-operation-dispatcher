
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "apsignal.h"

void set_timer(int inter, void (*handler)(int)) {
    signal(SIGALRM, handler);
    alarm(inter);
}

void set_intr_handler(void (*handler)(int)) {
    signal(SIGINT, handler);
}

void interrupt_process(int process_id) {
    kill(process_id, SIGINT);
}
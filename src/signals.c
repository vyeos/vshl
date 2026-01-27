#include "signals.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile sig_atomic_t g_got_sigint = 0;

void handle_sigint(int sig) {
    (void)sig; 
    g_got_sigint = 1;
    write(STDOUT_FILENO, "\n", 1);
}

int signals_consume_sigint(void) {
    if (g_got_sigint) {
        g_got_sigint = 0;
        return 1;
    }
    return 0;
}

void setup_parent_signals() {
    signal(SIGTSTP, SIG_IGN);
    
    struct sigaction sa;
    sa.sa_handler = &handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}

void restore_child_signals() {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

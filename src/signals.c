#include "signals.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handle_sigint(int sig) {
    (void)sig; 
    write(STDOUT_FILENO, "\n", 1);
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

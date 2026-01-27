#include "signals.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t g_got_sigint = 0;
static char g_prompt[1024];
static volatile sig_atomic_t g_prompt_len = 0;

void handle_sigint(int sig) {
    (void)sig; 
    g_got_sigint = 1;
    write(STDOUT_FILENO, "\n", 1);

    sig_atomic_t len = g_prompt_len;
    if (len > 0 && len < (sig_atomic_t)sizeof(g_prompt)) {
        write(STDOUT_FILENO, g_prompt, (size_t)len);
    }
}

int signals_consume_sigint(void) {
    if (g_got_sigint) {
        g_got_sigint = 0;
        return 1;
    }
    return 0;
}

void signals_set_prompt(const char *prompt) {
    if (prompt == NULL) {
        g_prompt_len = 0;
        g_prompt[0] = '\0';
        return;
    }

    size_t len = strnlen(prompt, sizeof(g_prompt) - 1);
    memcpy(g_prompt, prompt, len);
    g_prompt[len] = '\0';
    g_prompt_len = (sig_atomic_t)len;
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

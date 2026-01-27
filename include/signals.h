#ifndef SIGNALS_H
#define SIGNALS_H

void setup_parent_signals();
void restore_child_signals();
int signals_consume_sigint(void);

#endif
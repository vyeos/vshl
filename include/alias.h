#ifndef ALIAS_H
#define ALIAS_H

void alias_set(const char *name, const char *cmd);
const char *alias_get(const char *name);
void alias_print_all(void);

#endif

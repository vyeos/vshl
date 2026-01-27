#ifndef LINE_EDITING_H
#define LINE_EDITING_H

void line_editing_init(void);

void line_editing_shutdown(void);

char *read_command_line(const char *prompt);

void line_editing_record_line(const char *line);

#endif
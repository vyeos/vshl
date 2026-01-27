CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDLIBS = -ledit
SRC = src/main.c src/parsers.c src/builtins.c src/executors.c src/utils.c src/shell.c src/globbing.c src/signals.c src/jobs.c src/alias.c src/config/vshlrc.c src/line_editing.c
OBJ = $(SRC:.c=.o)
TARGET = vshl

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

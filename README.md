# vshl - A Custom Shell Implementation

A lightweight POSIX shell written in C with modern features like command history, tab completion, job control, and alias support.

## Features

### Core Functionality
- **Command Execution**: Execute system commands with full argument support
- **Pipelines**: Chain commands with `|` operator
- **Redirection**: Support for `>`, `>>`, `<`, and `2>` (stdout, append, stdin, stderr)
- **Logic Operators**: Sequential (`;`), conditional (`&&`, `||`) command execution
- **Background Jobs**: Run commands in background with `&` and manage with `jobs`, `fg`

### Interactive Features
- **Command History**: Navigate previous commands with arrow keys (stored in `~/.vshl_history`)
- **Tab Completion**: Auto-complete commands and filenames
- **Aliases**: Create command shortcuts with `alias` and `unalias`
- **Startup Config**: Auto-loads `~/.vshlrc` on launch for persistent settings

### Built-in Commands
- `cd [dir]` - Change directory (supports `~` expansion)
- `export VAR=value` - Set environment variables
- `unset VAR` - Remove environment variables
- `alias name="command"` - Create command alias
- `unalias name` - Remove alias
- `jobs` - List background jobs
- `fg [job_id]` - Bring job to foreground
- `exit [code]` - Exit shell

### Advanced Features
- **Globbing**: Wildcard expansion with `*` (match all) and `?` (match one)
- **Environment Expansion**: `$VAR`, `${VAR}`, and `~` expansion
- **Signal Handling**: Ctrl-C interrupts current job, Ctrl-Z suspends to background
- **Quote Handling**: Single and double quotes, backslash escaping

## Installation

### Prerequisites
- GCC compiler
- GNU Make
- libedit library (for macOS/BSD) or libreadline (for Linux)

### Build
```bash
make
```

### Run
```bash
./vshl
```

## Configuration

On first launch, vshl creates `~/.vshlrc` with default settings. Customize it with:

```bash
# Environment variables
export PATH="$PATH:$HOME/bin"
export EDITOR=vim

# Aliases
alias ll="ls -la"
alias gs="git status"
```

## Security

The codebase has been hardened against common vulnerabilities:
- Buffer overflow protection with bounds checking
- NULL pointer validation for all allocations
- Error handling for system calls (fork, dup2, kill)
- Safe signal handling with sig_atomic_t
- Input validation for environment variables and job IDs

## Limitations

- **Globbing in Quotes**: Wildcards expand even inside quotes (`ls "*.c"` behaves like `ls *.c`)
- **No Noclobber**: Output redirection (`>`) always overwrites files
- **Limited Expansion**: No command substitution, arithmetic expansion, or special parameters (`$?`, `$$`, etc.)
- **Tilde Expansion**: Only `~` (not `~user`) is supported

## Project Structure

```
include/          Header files
  ├── builtins.h
  ├── executor.h
  ├── globbing.h
  ├── jobs.h
  ├── parser.h
  ├── shell.h
  ├── signals.h
  └── utils.h
src/              Source files
  ├── builtins.c
  ├── executors.c
  ├── globbing.c
  ├── jobs.c
  ├── parsers.c
  ├── shell.c
  ├── signals.c
  ├── utils.c
  ├── main.c
  ├── alias.c
  └── line_editing.c
```

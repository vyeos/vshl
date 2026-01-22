# Flow

Input -> Check for logic chains -> Split on them -> builtins (if possible) -> split on pipeline or redirection -> start from check for logic chains

# New TODO

[-] working cd
[-] pipelines
[-] redirection (>, >>, < 2>)
[-] logic chaining (;, ||, &&)
[-] quoting (git commit -m "git commit" -> git and commit will go difeerently)
[-] escaping (make \ work -> escape characters)
[-] globbing (fix ls *.c -> * for match all, ? for match one character)
[-] signal handling (ctrl c should print new line instead of killing term, ctrl z pauses the process by keeping in mem alive in bg)
[-] job control (bg exec, process list, bring job to fg) 
[ ] env and expansion (env vars export, $, ~ expansion for everything curr only works in cd)
[ ] alias
[ ] startup config
[ ] raw mode (cmd his, tab autocomplete) 

# Far Fetched features
[ ] themes
[ ] autocorrect commands
[ ] trash (when running rm -rf, move the file/folder to .trash or somewhere and manage that using shell -> can be brought back by undo)
[ ] native zoxide (better to make a micro service or use zoxide)

# Limitations
- **Globbing inside quotes**: Wildcards are expanded even if they are inside quotes (e.g., `ls "*.c"` acts like `ls *.c`). This is because quotes are stripped during parsing before the glob expansion step. To fix this, the parser would need to preserve quote information or handle expansion during parsing.
- **Redirection**: Output redirection (`>`) overwrites files blindly (no `noclobber` -> is present in other shells). 

## Environment Expansion Limitations
- **Supported Expansions**:
  - `~`: Expands to `$HOME` (only at the start of a token).
  - `$VAR`: Expands to the value of the environment variable.
  - `${VAR}`: Expands to the value of the environment variable (braces are stripped).
- **Behavior**:
  - Undefined variables occuring in `$VAR` or `${VAR}` syntax expand to an empty string.
  - Buffer overflow protection is enforced; expansion truncates if the output buffer is too small.
- **Unsupported**:
  - `~user`: Expansion for specific users is not supported.
  - Command substitution (`$(cmd)` or backticks).
  - Arithmetic expansion (`$((...))`).
  - Nested expansion or special parameter handling (e.g., `$?`, `$!`, `$#`) is not currently implemented in this function. 


## file structure

include
 - builtins.h
 - executor.h
 - globbing.h
 - jobs.h
 - parser.h
 - shell.h
 - signals.h
 - utils.h
src
 - builtins.c
 - executor.c
 - globbing.c
 - jobs.c
 - parser.c
 - shell.c
 - signals.c
 - utils.c
Makefile

# Flow

Input -> Check for logic chains -> Split on them -> builtins (if possible) -> split on pipeline or redirection -> start from check for logic chains

# New TODO

[-] working cd
[-] pipelines
[-] redirection (>, >>, < 2>)
[-] logic chaining (;, ||, &&)
[-] quoting (git commit -m "git commit" -> git and commit will go difeerently)
[ ] escaping (make \ work -> escape characters)
[ ] globbing (fix ls *.c -> * for match all, ? for match one character)
[ ] signal handling (ctrl c should print new line instead of killing term, ctrl z pauses the process by keeping in mem alive in bg)
[ ] job control (bg exec, process list, bring job to fg) 
[ ] env and expansion (env vars export, $, ~ expansion for everything curr only works in cd)
[ ] alias
[ ] startup config
[ ] raw mode (cmd his, tab autocomplete) 

# Far Fetched features
  themes
[ ] autocorrect commands
[ ] trash (when running rm -rf, move the file/folder to .trash or somewhere and manage that using shell -> can be brought back by undo)
[ ] native zoxide

## fs

include
 - builtins.h
 - executor.h
 - parser.h
 - shell.h
src
 - builtins.c
 - executor.c
 - parser.c
 - shell.c

Makefile

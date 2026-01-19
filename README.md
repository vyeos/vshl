# vshl Roadmap

## Phase 1: The Skeleton (The Loop)
* [-] **Create the main C file** (e.g., `main.c`).
* [-] **Import necessary headers**:
    * `stdio.h` (for input/output)
    * `stdlib.h` (for memory allocation)
    * `unistd.h` (for fork, exec, pid_t)
    * `sys/types.h` (for pid_t)
    * `sys/wait.h` (for waitpid)
* [-] **Implement the Infinite Loop**: Create a `while` loop in `main` that runs forever.
* [-] **Print the Prompt**: Inside the loop, print something like `myshell> `.
* [-] **Compile & Run**: Ensure you can run it and see the prompt repeating (use `Ctrl+C` to kill it for now).

## Phase 2: Reading Input (The Reader)
* [-] **Allocate a Buffer**: Decide on a method (fixed array or dynamic `malloc`) to store user input.
* [-] **Read the Line**: Use a function like `getline` (recommended) or `fgets` to capture user input into your buffer.
* [-] **Handle EOF**: Check if the read function returned "End of File" (Ctrl+D). If so, break the loop to exit cleanly.
* [-] **Test Input**: Add a temporary `printf` to echo back what you typed, ensuring the buffer works.

## Phase 3: Parsing (The Tokenizer)
* [-] **Create a "Args" Array**: You need an array of strings ( `char *args[]` ) to hold the command and its flags.
* [-] **Split the String**: Use `strtok` (or write manual logic) to split the input line wherever there is a space.
* [-] **Store Tokens**:
    * Store the first word (e.g., "ls") in `args[0]`.
    * Store arguments (e.g., "-la") in `args[1]`, etc.
* [-] **Null Terminate**: IMPORTANT—Set the last element of your array to `NULL`. `execvp` needs this to know where the list ends.

## Phase 4: Execution (The Engine)
* [-] **Call `fork()`**: Inside the loop, fork the process.
* [-] **Check PID**: Write an `if/else` block to distinguish between the **Child** (pid == 0) and the **Parent** (pid > 0).
* [-] **Implement Child Logic**:
    * Call `execvp(args[0], args)`.
    * Add error handling: If `execvp` returns, it means it failed (command not found). Print an error and `exit()`.
* [-] **Implement Parent Logic**:
    * Call `waitpid()` to pause execution until the child is finished.
    * Check if `fork()` returned `< 0` (system error) and handle it.

## Phase 5: Built-in Commands
* [-] **Handle "exit"**: Before forking, check if `args[0]` is "exit". If yes, break the main loop or `exit(0)`.
    * *Note: You cannot run "exit" via `execvp` because `execvp` runs external programs, and "exit" is a command for your shell itself.*

## Phase 6: Memory Cleanup (Good Habits)
* [-] **Free Memory**: If you used `malloc` for buffers or arrays, ensure you `free` them at the end of every loop iteration to prevent leaks.

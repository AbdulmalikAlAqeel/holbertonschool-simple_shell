#  Simple Shell

A UNIX command line interpreter written in C.

------------------------------------------------------------------------

## 📌 Project Overview

This project is a minimalist implementation of a UNIX shell. It
replicates the core behavior of `/bin/sh` using low-level system calls
without relying on external libraries beyond the standard C library.

The shell supports:

-   Interactive and non-interactive mode
-   Execution of commands with arguments
-   PATH resolution
-   Built-in commands (`exit`, `env`)
-   Proper error handling
-   Correct exit status propagation
-   Clean memory management (Valgrind safe)

------------------------------------------------------------------------

## 📚 Learning Objectives

Through this project we practiced:

-   Process creation (`fork`)
-   Program execution (`execve`)
-   Process synchronization (`wait`)
-   Environment handling (`environ`)
-   PATH parsing
-   Dynamic memory management
-   System call error handling
-   UNIX process lifecycle

------------------------------------------------------------------------

## 🛠 Compilation

``` bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

------------------------------------------------------------------------

## 🚀 Usage

### Interactive Mode

``` bash
$ ./hsh
$ ls
$ ls -l /tmp
$ env
$ exit
```

### Non-Interactive Mode

``` bash
echo "ls -l" | ./hsh
```

------------------------------------------------------------------------

# ⚙️ Features Implemented

## ✅ Basic Shell

-   Displays a prompt in interactive mode
-   Reads input using `getline`
-   Executes commands using `fork` and `execve`
-   Handles EOF (`Ctrl + D`)

------------------------------------------------------------------------

## ✅ Handle Arguments

Supports commands with arguments:

``` bash
ls -l /var
```

Tokenization is implemented using `strtok`.

------------------------------------------------------------------------

## ✅ Handle PATH

If the command does not contain `/` or `.`, the shell searches for it in
the `PATH` environment variable.

Example:

``` bash
ls
```

Internally searches directories like:

-   `/bin/ls`
-   `/usr/bin/ls`

If not found → prints error without calling `fork`.

------------------------------------------------------------------------

## ✅ Built-in: exit

Usage:

``` bash
exit
```

-   Exits the shell
-   Returns the last command exit status
-   No arguments required

------------------------------------------------------------------------

## ✅ Built-in: env

Usage:

``` bash
env
```

Prints all environment variables using:

``` c
extern char **environ;
```

------------------------------------------------------------------------

# 🧠 Execution Flow

1.  Display prompt (if interactive)
2.  Read input (getline)
3.  Remove newline
4.  Tokenize input
5.  Check built-ins (`exit`, `env`)
6.  Search in PATH (if needed)
7.  If command not found → print error (NO fork)
8.  `fork()`
9.  Child → `execve()`
10. Parent → `wait()`
11. Return exit status
12. Repeat

------------------------------------------------------------------------

# 📂 Main Components

### 🔹 main()

-   Shell loop
-   Input parsing
-   Built-in handling
-   Status tracking

### 🔹 execute_command()

-   Validates executable path
-   Forks process
-   Calls `execve`
-   Waits for child
-   Returns exit status

### 🔹 find_in_path()

-   Parses `PATH`
-   Iterates directories
-   Constructs full path
-   Validates using `stat`
-   Returns full path or NULL

### 🔹 print_env()

-   Iterates through `environ`
-   Prints environment variables

------------------------------------------------------------------------

# ❌ Error Handling

Handles:

-   Command not found
-   Fork failure
-   Execve failure
-   Empty input
-   EOF
-   Memory allocation errors

Error format:

    ./hsh: line_number: command: not found

Exit status `127` is returned when command is not found.

------------------------------------------------------------------------

# 🧵 Memory Management

-   All dynamically allocated memory is freed
-   `cmd_path` freed after use
-   `line` freed before exit
-   No memory leaks (Valgrind clean)

------------------------------------------------------------------------

# 🔐 System Calls Used

-   fork
-   execve
-   wait
-   write
-   access
-   stat
-   getline
-   malloc
-   free

------------------------------------------------------------------------

# 🧾 Return Status Behavior

-   Returns the exit status of the last executed command
-   Built-in `exit` returns last status

------------------------------------------------------------------------

# 👨‍💻 Author

AZZAM AL DUYULI && ABDULMALIK BIN AQEEL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

extern char **environ;

/* Function Prototypes */
char *find_in_path(char *cmd);
void execute_command(char **argv, int line_num, char *line);

/**
 * main - Entry point for the simple shell.
 * 
 * Description: Reads user input, parses it, checks for built-ins
 * like 'exit', and executes external commands found in PATH.
 * 
 * Return: Always 0 (Success).
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *argv[64];
	int i, line_num = 0;

	while (1)
	{
		line_num++;
		/* Print prompt only in interactive mode */
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);

		nread = getline(&line, &len, stdin);
		if (nread == -1) /* Handle Ctrl+D (EOF) */
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		/* Remove newline character from input */
		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		/* Tokenize input string into arguments */
		i = 0;
		argv[i] = strtok(line, " ");
		while (argv[i] && i < 63)
			argv[++i] = strtok(NULL, " ");

		if (argv[0] == NULL) /* Handle empty input */
			continue;

		/* TASK 0.4: Implement 'exit' built-in */
		if (strcmp(argv[0], "exit") == 0)
		{
			free(line); /* Clean memory before exiting */
			exit(0);
		}

		execute_command(argv, line_num, line);
	}
	free(line);
	return (0);
}

/**
 * execute_command - Forks a child process and executes a command.
 * @argv: Array of arguments (the command and its flags).
 * @line_num: The count of commands entered (for error reporting).
 * @line: The input buffer to free in the child process if execve fails.
 */
void execute_command(char **argv, int line_num, char *line)
{
	pid_t pid;
	char *cmd_path = NULL;
	int status;

	/* Determine if command is a full path or needs PATH search */
	if (argv[0][0] != '/' && argv[0][0] != '.')
		cmd_path = find_in_path(argv[0]);
	else if (access(argv[0], X_OK) == 0)
		cmd_path = strdup(argv[0]);

	if (cmd_path == NULL)
	{
		fprintf(stderr, "./hsh: %d: %s: not found\n", line_num, argv[0]);
		return;
	}

	pid = fork();
	if (pid == 0) /* Child Process */
	{
		if (execve(cmd_path, argv, environ) == -1)
		{
			perror("execve");
			free(cmd_path);
			free(line);
			exit(127);
		}
	}
	else if (pid < 0) /* Fork failed */
	{
		perror("fork");
		free(cmd_path);
	}
	else /* Parent Process */
	{
		wait(&status);
		free(cmd_path);
	}
}

/**
 * find_in_path - Searches for a command in the PATH environment variable.
 * @cmd: The command to find (e.g., "ls").
 * 
 * Return: The full path to the executable if found, or NULL.
 */
char *find_in_path(char *cmd)
{
	char *path = NULL, *copy, *dir, *full;
	int i = 0;
	struct stat st;

	/* Locate PATH in environment variables */
	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
		{
			path = environ[i] + 5;
			break;
		}
		i++;
	}

	if (!path || path[0] == '\0')
		return (NULL);

	copy = strdup(path);
	dir = strtok(copy, ":");
	while (dir)
	{
		/* Allocate space for: dir + '/' + cmd + '\0' */
		full = malloc(strlen(dir) + strlen(cmd) + 2);
		sprintf(full, "%s/%s", dir, cmd);

		if (stat(full, &st) == 0) /* Check if file exists */
		{
			free(copy);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}
	free(copy);
	return (NULL);
}

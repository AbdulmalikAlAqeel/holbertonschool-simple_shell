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
int execute_command(char **argv, int line_num, char *line);

/**
 * main - Entry point for the simple shell.
 * 
 * Return: The last command's exit status.
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *argv[64];
	int i, line_num = 0;
	int last_status = 0; /* To track the exit code of commands */

	while (1)
	{
		line_num++;
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		i = 0;
		argv[i] = strtok(line, " ");
		while (argv[i] && i < 63)
			argv[++i] = strtok(NULL, " ");

		if (argv[0] == NULL)
			continue;

		/* TASK 0.4: exit with the last command's status */
		if (strcmp(argv[0], "exit") == 0)
		{
			free(line);
			exit(last_status);
		}

		last_status = execute_command(argv, line_num, line);
	}
	free(line);
	return (last_status);
}

/**
 * execute_command - Executes command and returns its exit status.
 * @argv: Arguments array.
 * @line_num: Line number for errors.
 * @line: Input buffer.
 * 
 * Return: Exit status of the command.
 */
int execute_command(char **argv, int line_num, char *line)
{
	pid_t pid;
	char *cmd_path = NULL;
	int status, exit_code = 0;

	if (argv[0][0] != '/' && argv[0][0] != '.')
		cmd_path = find_in_path(argv[0]);
	else if (access(argv[0], X_OK) == 0)
		cmd_path = strdup(argv[0]);

	if (cmd_path == NULL)
	{
		fprintf(stderr, "./hsh: %d: %s: not found\n", line_num, argv[0]);
		return (127); /* Command not found status */
	}

	pid = fork();
	if (pid == 0) /* Child */
	{
		if (execve(cmd_path, argv, environ) == -1)
		{
			free(cmd_path);
			free(line);
			exit(127);
		}
	}
	else if (pid < 0)
	{
		perror("fork");
		free(cmd_path);
		return (1);
	}
	else /* Parent */
	{
		wait(&status);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		free(cmd_path);
	}
	return (exit_code);
}

/**
 * find_in_path - Searches PATH for command.
 * @cmd: command name.
 * Return: Full path or NULL.
 */
char *find_in_path(char *cmd)
{
	char *path = NULL, *copy, *dir, *full;
	int i = 0;
	struct stat st;

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
		full = malloc(strlen(dir) + strlen(cmd) + 2);
		sprintf(full, "%s/%s", dir, cmd);
		if (stat(full, &st) == 0)
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

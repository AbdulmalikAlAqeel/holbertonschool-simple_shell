#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>


extern char **environ;

void handle_sigint(int sig);
char *find_in_path(char *cmd);
int execute_command(char **argv, int line_num, char *line);
void print_env(void);

/**
 * main - Entry point for the simple shell.
 * Return: Exit status of the last executed command.
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *argv[64];
	int i, line_num = 0, last_status = 0;

	signal(SIGINT, handle_sigint);
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

		/* TASK 0.4: Handle exit built-in */
		if (strcmp(argv[0], "exit") == 0)
		{
			free(line);
			exit(last_status);
		}
		/* TASK 1.0: Handle env built-in */
		else if (strcmp(argv[0], "env") == 0)
		{
			print_env();
			last_status = 0;
			continue;
		}

		last_status = execute_command(argv, line_num, line);
	}
	free(line);
	return (last_status);
}

/**
 * print_env - Prints all environment variables to stdout.
 * 
 * Description: Iterates through the global 'environ' array
 * and writes each string followed by a newline.
 */
void print_env(void)
{
	int i = 0;

	while (environ[i])
	{
		write(STDOUT_FILENO, environ[i], strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/**
 * execute_command - Forks and executes a command.
 * @argv: Array of arguments.
 * @line_num: Command count for error messages.
 * @line: Input buffer to free in child if execve fails.
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
		return (127);
	}

	pid = fork();
	if (pid == 0) /* Child process */
	{
		if (execve(cmd_path, argv, environ) == -1)
		{
			free(cmd_path);
			free(line);
			exit(127);
		}
	}
	else if (pid < 0) /* Fork failure */
	{
		perror("fork");
		free(cmd_path);
		return (1);
	}
	else /* Parent process */
	{
		wait(&status);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		free(cmd_path);
	}
	return (exit_code);
}

/**
 * find_in_path - Searches for a command in PATH directories.
 * @cmd: The command to find.
 * Return: Full path to command if found, NULL otherwise.
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

/**
 * handle_sigint - Prints all environment variables to stdout.
 * @sig: int value
 * Description: Iterates through the global 'environ' array
 * and writes each string followed by a newline.
 */
void handle_sigint(int sig)
{
    (void)sig;
}

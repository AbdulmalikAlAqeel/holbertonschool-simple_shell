#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


/**
 * main - entry point
 * Return: always (0) success
 */
int main()
{
pid_t pid;
ssize_t read;
size_t len = 0;
char *argv[64];
char *token;
int status;
int i;
char full_path[1024];
int exit_status = 0;
char *line = NULL;


while (1)
{
i = 0;
if (isatty(STDIN_FILENO))
{
printf("$ ");
}
read = getline(&line, &len, stdin);
if (read == -1)
{
if (isatty(STDIN_FILENO))
{
printf("\n");
}
break;
}
line[strcspn(line, "\n")] = '\0';
token = strtok(line, " ");
while (token)
{
argv[i++] = token;
token = strtok(NULL, " ");
}
argv[i] = NULL;
if (argv[0] == NULL)
{
continue;
}
pid = fork();
if (pid == 0)
{
if (argv[0][0] != '/')
{
strcpy(full_path, "/bin/");
strcat(full_path,argv[0]);
argv[0] = full_path;
}
execve(argv[0], argv, NULL);
free(line);
exit(127);
}
else
{
wait(&status);
if (WIFEXITED(status))
    exit_status = WEXITSTATUS(status);
}
}
free(line);
return (exit_status);
}

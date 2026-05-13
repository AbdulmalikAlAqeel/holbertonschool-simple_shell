#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
pid_t my_pid;
int i = 0;
ssize_t read;
char *argv[64];
char *token;
char *line = NULL;
size_t len = 0;
while (true)
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
my_pid = fork();
if (my_pid == 0)
{
token = strtok(line, " ");
while (token)
{
argv[i++] = token;
token = strtok(NULL, " ");
}
argv[i] = NULL;
if (argv[0])
{
execve(argv[0], argv, NULL);
printf("./shell: No such file or directory\n");
}
exit(EXIT_FAILURE);
}
else
{
    wait(NULL);
}
}
return (1);
}
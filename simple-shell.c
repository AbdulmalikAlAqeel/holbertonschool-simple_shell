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
int error;
int i = 0;
ssize_t read;
char *argv[64];
char *token;
char *line = NULL;
size_t len = 0;
while (true)
{
if (isatty(STDIN_FILENO))
{
printf("$ ");  
read = getline(&line, &len, stdin);
if (read == -1)
{
break;
} 
}
read = getline(&line, &len, stdin);
if (read == -1)
{
printf("\n");
break;
}
my_pid = fork();
if (my_pid == 0)
{
line[strcspn(line, "\n")] = '\0';
token = strtok(line, " ");
while (token)
{
    argv[i++] = token;
    token = strtok(NULL, " ");
}
argv[i] = NULL;
if (strlen(line) > 1)
{
error = execve(argv[0], argv, NULL);
if (error == -1)
{
printf("./shell: No such file or directory\n");
}
}
break;
}
else
{
wait(NULL);
}

}
free(line);
return (0);
}

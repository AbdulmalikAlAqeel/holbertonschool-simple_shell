#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *find_in_path(char *cmd);

int main(void)
{
    pid_t pid;
    ssize_t read;
    size_t len = 0;
    char *argv[64];
    char *token;
    int status;
    int i;
    int exit_status = 0;
    char *line = NULL;

    while (1)
    {
        i = 0;
        if (isatty(STDIN_FILENO))
        {
            printf("$ ");
            fflush(stdout);
        }
        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            if (isatty(STDIN_FILENO))
                printf("\n");
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue;
        token = strtok(line, " ");
        while (token && i < 63)
        {
            argv[i++] = token;
            token = strtok(NULL, " ");
        }
        argv[i] = NULL;
        if (argv[0] == NULL)
            continue;
        if (argv[0][0] != '/' && argv[0][0] != '.')
        {
            char *cmd_path = find_in_path(argv[0]);
            if (cmd_path == NULL)
            {
                exit_status = 127;
                continue;
            }
            free(cmd_path);
        }
        else
        {
            if (access(argv[0], X_OK) != 0)
            {
                exit_status = 127;
                continue;
            }
        }
        pid = fork();
        if (pid == -1)
        {
            exit_status = 1;
            continue;
        }
        if (pid == 0)
        {
            char *cmd_copy = NULL;
            if (argv[0][0] != '/' && argv[0][0] != '.')
            {
                cmd_copy = find_in_path(argv[0]);
                if (cmd_copy == NULL)
                {
                    free(line);
                    exit(127);
                }
                argv[0] = cmd_copy;
            }
            execve(argv[0], argv, NULL);
            if (cmd_copy)
                free(cmd_copy);
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

char *find_in_path(char *cmd)
{
    char *path = getenv("PATH");
    char *copy;
    char *dir;
    if (path == NULL || path[0] == '\0')
        return NULL;
    copy = strdup(path);
    if (copy == NULL)
        return NULL;
    dir = strtok(copy, ":");
    while (dir)
    {
        char *full = malloc(strlen(dir) + strlen(cmd) + 2);
        if (full == NULL)
        {
            free(copy);
            return NULL;
        }
        snprintf(full, strlen(dir) + strlen(cmd) + 2, "%s/%s", dir, cmd);
        if (access(full, X_OK) == 0)
        {
            free(copy);
            return full;
        }
        free(full);
        dir = strtok(NULL, ":");
    }
    free(copy);
    return NULL;
}
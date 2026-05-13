#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *find_in_path(char *cmd);

int main(int argc, char **argv_main, char **envp)
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
    char *cmd_path;

    (void)argc;
    (void)argv_main;

    while (1)
    {
        i = 0;

        if (isatty(STDIN_FILENO))
            printf("$ ");

        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            if (isatty(STDIN_FILENO))
                printf("\n");
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
            continue;

        if (strchr(argv[0], '/'))
        {
            cmd_path = argv[0];
        }
        else
        {
            cmd_path = find_in_path(argv[0]);
            if (cmd_path == NULL)
            {
                exit_status = 127;
                continue;
            }
            argv[0] = cmd_path;
        }

        pid = fork();
        if (pid == 0)
        {
            execve(argv[0], argv, envp);
            exit(127);
        }
        else
        {
            wait(&status);
            if (WIFEXITED(status))
                exit_status = WEXITSTATUS(status);
        }

        if (!strchr(argv[0], '/') && cmd_path != NULL)
            free(cmd_path);
    }

    free(line);
    return (exit_status);
}

char *find_in_path(char *cmd)
{
    char *path;
    char *copy;
    char *dir;
    char full[1024];

    path = getenv("PATH");
    if (path == NULL || path[0] == '\0')
        return NULL;

    copy = strdup(path);
    if (!copy)
        return NULL;

    dir = strtok(copy, ":");
    while (dir)
    {
        snprintf(full, sizeof(full), "%s/%s", dir, cmd);
        if (access(full, X_OK) == 0)
        {
            free(copy);
            return strdup(full);
        }
        dir = strtok(NULL, ":");
    }

    free(copy);
    return NULL;
}
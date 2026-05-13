#include "main.h"

/**
 * _which - Locates a command in the PATH directories.
 * @command: The name of the command to find (e.g., "ls", "pwd").
 * 
 * Return: Full path to the command if found, or NULL if not found.
 */
char *_which(char *command)
{
    char *path, *path_copy, *token;
    char *full_path;
    struct stat st;
    int cmd_len, dir_len;

    /* 1. Retrieve the PATH environment variable */
    path = _getenv("PATH");
    if (!path)
        return (NULL);

    /* 2. Create a copy of PATH to avoid modifying the original string */
    path_copy = _strdup(path);
    if (!path_copy)
        return (NULL);

    cmd_len = _strlen(command);

    /* 3. Tokenize the PATH string into individual directories using ':' */
    token = strtok(path_copy, ":");
    while (token != NULL)
    {
        dir_len = _strlen(token);
        
        /* Allocate memory: dir_len + '/' + cmd_len + '\0' */
        full_path = malloc(dir_len + cmd_len + 2);
        if (!full_path)
        {
            free(path_copy);
            return (NULL);
        }

        /* Construct the potential full path (e.g., "/bin" + "/" + "ls") */
        _strcpy(full_path, token);
        _strcat(full_path, "/");
        _strcat(full_path, command);

        /* 4. Check if the constructed path exists and is a valid file */
        if (stat(full_path, &st) == 0)
        {
            free(path_copy);
            return (full_path); /* Command found! */
        }

        /* Clean up if not found in this directory, then move to the next */
        free(full_path);
        token = strtok(NULL, ":");
    }

    /* Clean up if the command wasn't found in any directory */
    free(path_copy);
    return (NULL);
}

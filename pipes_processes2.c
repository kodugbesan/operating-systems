
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int pipe1[2];
    int pipe2[2];

    pid_t pid1;
    pid_t pid2;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        return 1;
    }

    // Pipe from cat to grep
    if (pipe(pipe1) == -1)
    {
        perror("pipe");
        return 1;
    }

    // Pipe from grep to sort
    if (pipe(pipe2) == -1)
    {
        perror("pipe");
        return 1;
    }

    // Create child process for grep
    pid1 = fork();

    if (pid1 < 0)
    {
        perror("fork");
        return 1;
    }

    // Child process: grep
    if (pid1 == 0)
    {
        // Create grandchild process for sort
        pid2 = fork();

        if (pid2 < 0)
        {
            perror("fork");
            return 1;
        }

        // Grandchild process: sort
        if (pid2 == 0)
        {
            dup2(pipe2[0], STDIN_FILENO);

            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            execlp("sort", "sort", NULL);

            perror("execlp sort");
            exit(1);
        }

        // Child process: grep
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("grep", "grep", argv[1], NULL);

        perror("execlp grep");
        exit(1);
    }

    // Parent process: cat
    dup2(pipe1[1], STDOUT_FILENO);

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    execlp("cat", "cat", "scores", NULL);

    perror("execlp cat");
    return 1;
}

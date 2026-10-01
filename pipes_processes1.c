
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2];
    int fd2[2];

    char fixed_str[] = "howard.edu";
    char final_str[] = "gobison.org";

    char input_str[100];
    char second_input[100];
    char concat_str[300];

    pid_t p;

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }

    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }

    printf("Enter a string to concatenate:");
    scanf("%99s", input_str);

    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed\n");
        return 1;
    }

    // Parent process
    else if (p > 0)
    {
        close(fd1[0]);
        close(fd2[1]);

        // Send input string to child
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        // Receive final string from child
        read(fd2[0], concat_str, sizeof(concat_str));

        // Parent adds gobison.org
        strcat(concat_str, final_str);

        printf("Output : %s\n", concat_str);

        close(fd2[0]);

        wait(NULL);
    }

    // Child process
    else
    {
        close(fd1[1]);
        close(fd2[0]);

        // Read string from parent
        read(fd1[0], concat_str, sizeof(concat_str));
        close(fd1[0]);

        // Add howard.edu
        strcat(concat_str, fixed_str);

        printf("Other string is: %s\n", fixed_str);
        printf("Input : %s\n", input_str);
        printf("Output : %s\n", concat_str);

        // Get second input
        printf("Input : ");
        scanf("%99s", second_input);

        // Add second input
        strcat(concat_str, second_input);

        // Send string back to parent
        write(fd2[1], concat_str, strlen(concat_str) + 1);

        close(fd2[1]);

        exit(0);
    }

    return 0;
}

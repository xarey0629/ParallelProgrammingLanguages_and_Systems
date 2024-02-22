#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid;

    pid = fork(); /* fork another process */

    if (pid < 0)
    { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1; // exit(1);
    }

    else if (pid == 0)
    { /* child process */
        int num = 0;
        printf("Please input a positive integer N: ");
        while (scanf("%d", &num))
        {
            if (num > 0)
            {
                break;
            }
            printf("Error: Invalid input. Please enter a positive integer: ");
            fflush(stdin);
        }
        printf("%i", num);
        while (num != 1)
        {
            if (num % 2 == 0) // odd
            {
                num /= 2;
                printf(" %i", num); //
            }
            else
            {
                num = num * 3 + 1;
                printf(" %i", num); //
            }
        }
        printf("\n");
    }

    else
    { /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL);
        printf("Child Complete\n");
    }
    return 0; // exit(0);
}

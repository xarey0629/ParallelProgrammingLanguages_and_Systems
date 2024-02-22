#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    // Create shared memory
    const int SIZE = 4096;
    const char *name = "OS";

    int shm_fd; // shared memory file descriptor
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    // fork
    pid_t pid;
    pid = fork(); /* fork another process */

    if (pid < 0)
    { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1; // exit(1);
    }

    else if (pid == 0)
    { /* child process */

        // pointer to shared memory object
        int *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

        // User Input Section
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

        // Store into memory.
        *ptr++ = num;
        while (num != 1)
        {
            if (num % 2 == 0) // odd
            {
                num /= 2;
                *ptr++ = num;
            }
            else // even
            {
                num = num * 3 + 1;
                *ptr++ = num;
            }
        }
        printf("Child process completed writing.\n");
    }

    else
    { /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL);
        printf("Parent process is taking control.\n");

        // Map to shared memory
        int *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

        printf("The sequence is:");
        while (*ptr != 0)
        {
            printf(" %i", *ptr++);
        }

        // Remove shared memory object.
        shm_unlink(name);
    }
    return 0; // exit(0);
}
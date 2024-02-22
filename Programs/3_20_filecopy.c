#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096
#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];

    // create a pipe
    int fd[2];
    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    // fork()
    pid_t pid;
    pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork failed");
        return 1;
    }
    else if (pid > 0)
    { // parent process
        close(fd[READ_END]);

        printf("Parent's searching file named: %s.\n", argv[1]);
        FILE *infile = fopen(argv[1], "r");
        if (infile == NULL)
        {
            printf("Error: input file not found.\n");
            return 1;
        }
        printf("Success! Parent's sending message to pipe.\n...\n");

        // Get the size of the file
        fseek(infile, 0L, SEEK_END);
        long fileSize = ftell(infile);
        printf("Input file detect %li words.\n", fileSize);
        rewind(infile);

        // Read the file into write_msg.
        fread(write_msg, fileSize, 1, infile);
        write(fd[WRITE_END], write_msg, fileSize + 1); // EOF is added, so add filesize by 1.
        close(fd[WRITE_END]);
        fclose(infile);
        printf("Parent input finished.\n");
    }
    else
    { // child process
        close(fd[WRITE_END]);

        FILE *outfile = fopen(argv[2], "w");
        if (outfile == NULL)
        {
            printf("Error: opening output file failed\n");
            return 1;
        }

        printf("Child's trying to copy the file.\n");
        read(fd[READ_END], read_msg, BUFFER_SIZE);                 // Load content from pipe into read_msg
        fwrite(read_msg, sizeof(char), strlen(read_msg), outfile); // Outout to the copy.txt
        printf("Number of words to be copied: %lu words.\n...\n", strlen(read_msg));

        close(fd[READ_END]);
        fclose(outfile);
        printf("Child successes copying.\nOutput file: %s.", argv[2]);
    }

    return 0;
}
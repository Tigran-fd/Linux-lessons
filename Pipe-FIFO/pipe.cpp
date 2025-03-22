#include <unistd.h>
#include <cstdio>
#include <cstring>

int main(int argc, char** argv)
{
    // If pipe creation is successful,
    // fd[0] -> read
    // fd[1] -> write
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("Error occurred during pipe creation: ");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Error occurred during fork: ");
        return 1;
    }
    if (pid == 0)
    {
        close(fd[0]);

        char* msg = "Hello, Parent!";
        printf("%s\n", "Child: Writing a message to a parent.");
        write(fd[1], msg, strlen(msg));
        
        close(fd[1]);
        return 0;
    }

    close(fd[1]);

    printf("%s\n", "Parent: Reading a message from a child.");

    char c;
    while (read(fd[0], &c, 1) > 0)
    {
        // Read a byte from pipe and write it to STD out
        write(1, &c, 1);
    }

    close(fd[0]);

    return 0;
}
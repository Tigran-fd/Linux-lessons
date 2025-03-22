#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void handle(int signo)
{
    if (signo == SIGINT)
    {
        char* msg = "Interrupt signal is caught!\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
}

int main(int argc, char** argv)
{
    if (signal(SIGINT, handle) == SIG_ERR)
    {
        perror("Unable to register a signal handler.");
        return 1;
    }
    sleep(100);
    return 0;
}
#include <signal.h>
#include <string.h>
#include <strings.h>
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
    struct sigaction act;
    bzero(&act, sizeof(act));

    act.sa_handler = handle;
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("Unable to register the signal handler.");
        return 1;
    }

    sleep(100);
    return 0;
}

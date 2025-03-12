#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

void handle(int signo, siginfo_t *info, void *ucontext)
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

    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = handle;
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("Unable to register the signal handler.");
        return 1;
    }

    for (int count = sleep(5); count != 0; count = sleep(count));
    return 0;
}

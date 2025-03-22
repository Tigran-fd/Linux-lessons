#include <csignal>
#include <cstring>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <iostream>

// For testing, use SIGINT 100
// Find signal <pid> with ps aux | grep signale_handle
// Then run kill -2 <pid>

std::unordered_map<std::string, int> SIGNAL_MAP = {
    {"SIGHUP", 1},   {"SIGINT", 2},   {"SIGQUIT", 3},  {"SIGILL", 4},   {"SIGTRAP", 5},
    {"SIGABRT", 6},  {"SIGIOT", 6},   {"SIGBUS", 7},   {"SIGFPE", 8},   {"SIGKILL", 9},
    {"SIGUSR1", 10}, {"SIGSEGV", 11}, {"SIGUSR2", 12}, {"SIGPIPE", 13}, {"SIGALRM", 14},
    {"SIGTERM", 15}, {"SIGSTKFLT", 16},{"SIGCHLD", 17}, {"SIGCONT", 18}, {"SIGSTOP", 19},
    {"SIGTSTP", 20}, {"SIGTTIN", 21}, {"SIGTTOU", 22}, {"SIGURG", 23},  {"SIGXCPU", 24},
    {"SIGXFSZ", 25}, {"SIGVTALRM", 26},{"SIGPROF", 27}, {"SIGWINCH", 28},{"SIGIO", 29},
    {"SIGPOLL", 29}, {"SIGPWR", 30},  {"SIGSYS", 31},  {"SIGUNUSED", 31}
};

void handle(int signo, siginfo_t *info, void *ucontext)
// I don't use siginfo_t and ucontext; they can be removed. However, since they are part of the example, I'll leave them in.
{
    std::cout << "Signal " << signo << " received.\n";
    char* msg = "Interrupt signal is caught!";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <signal_name> <seconds>" << std::endl;
        return 1;
    }

    std::string signal_name = argv[1];
    const int wait_time = std::atoi(argv[2]);

    if (SIGNAL_MAP.find(signal_name) == SIGNAL_MAP.end()) {
        std::cerr << "Unknown signal." << std::endl;
        return 1;
    }

    int signal_number = SIGNAL_MAP[signal_name];

    struct sigaction act{};
    bzero(&act, sizeof(act));
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    // I don't need these flags in this program, but since they are in the example, I will leave them as is.
    act.sa_sigaction = handle;

    if (sigaction(signal_number, &act, nullptr) == -1)
    {
        perror("Unable to register the signal handler.");
        return 1;
    }
    sleep(wait_time);
    return 0;
}

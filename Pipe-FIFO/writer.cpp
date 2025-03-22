#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("%s", "Usage error.\n");
        return 1;
    }

    const char* path = "/tmp/fifo_007";
    if (mkfifo(path, 0666) == -1) {
        perror("fifo make");
        return 1;
    }

    const int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("fifo open");
        return 1;
    }

    // argv    is char**
    // argv[1] is char*
    write(fd, argv[1], strlen(argv[1]));
    printf("%s", "Write Done");

    close(fd);
    printf("%s", "Write Done");
    
    return 0;
}
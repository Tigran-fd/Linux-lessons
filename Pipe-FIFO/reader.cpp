#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    const char* path = "/tmp/fifo_007";
    const int   fd   = open(path, O_RDONLY);
    if (fd == -1) {
        perror("fifo open");
        return 1;
    }
    
    const char* msg = "Message from a writer:";
    write(STDOUT_FILENO, msg, strlen(msg));

    char buf;
    while (read(fd, &buf, 1) > 0)
        write(STDOUT_FILENO, &buf, 1);
    
    write(STDOUT_FILENO, "\n", 1);
    
    close(fd);
    unlink(path);

    return 0;
}
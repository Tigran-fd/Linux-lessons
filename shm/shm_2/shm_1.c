// C STD headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <sys/shm.h>

int main(int argc, char** argv)
{
    key_t  key   = 1897;
    size_t size  = 1024;
    int    shmId = shmget(key, size, IPC_EXCL | IPC_CREAT | 0666);
    if (shmId == -1)
    {
        perror("SHM1: Error occurred during getting/creating : ");
        return 1;
    }

    void* ptr = shmat(shmId, NULL, 0);
    if (ptr == (void*)-1)
    {
        perror("SHM1: Error occured during attachment: ");
        return 1;
    }

    printf("%s\n", "SHM1: Assigning elements...");
    int* intPtr = ptr;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        intPtr[i] = i;
    }

    if (shmdt(ptr) == -1)
    {
        perror("SHM1: Error occurred during detachment: ");
        return 1;
    }

    return 0;
}

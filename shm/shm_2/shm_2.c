// C STD headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <sys/shm.h>

int main(int argc, char** argv)
{
    key_t  key   = 1897;
    size_t size  = 1024;
    int    shmId = shmget(key, size, IPC_CREAT | 0666);
    if (shmId == -1)
    {
        perror("SHM2: Error occurred during getting/creating : ");
        return 1;
    }

    void* ptr = shmat(shmId, NULL, 0);
    if (ptr == (void*)-1)
    {
        perror("SHM2: Error ocurred during detachment: ");
        return 1;
    }

    printf("%s\n", "SHM2: Reading from SHM1-created shared memory: ");
    
    int* intPtr = ptr;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        printf("%d ", intPtr[i]);
    }
    printf("\n");

    if (shmdt(ptr) == -1)
    {
        perror("SHM2: Error occured duing detachment: ");
        return 1;
    }
    if (shmctl(shmId, IPC_RMID, NULL) == -1)
    {
        perror("SHM2: Error occurred during removal: ");
        return 1;
    }

    return 0;
}

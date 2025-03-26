// C STD headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <sys/shm.h>

int main(int argc, char** argv)
{
    key_t  key   = 1897;
    size_t size  = 1024;
    
    // 0666
    // 110 110 110
    // RWE RWE RWE    
    int shmId = shmget(key, size, IPC_EXCL | IPC_CREAT | 0666);
    if (shmId == -1)
    {
        perror("Error occurred during getting/creating : ");
        return 1;
    }

    // kernel address
    // 0xF8C0
    // .
    // .
    // .
    // 0XF8CD

    // process address space
    // 0xC7C0 -> 0xF8C0
    // 0xC7C1 -> 0xF8C1
    // .
    // .
    // .
    // 0xC7CD -> 0xF8CD
    void* ptr = shmat(shmId, NULL, 0);
    if (ptr == (void*)-1)
    {
        perror("Error occured during attachment: ");
        return 1;
    }

    printf("%s\n", "Assigning elements...");
    int* intPtr = ptr;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        intPtr[i] = i;
    }

    printf("%s\n", "Printing elements...");
    for (int i = 0; i < size / sizeof(int); i++)
    {
        printf("%d ", intPtr[i]);
    }
    printf("\n");

    if (shmdt(ptr) == -1)
    {
        perror("Error occurred during detachment: ");
        return 1;
    }

    if (shmctl(shmId, IPC_RMID, NULL) == -1)
    {
        perror("Error occured during removal: ");
        return 1;
    }

    return 0;
}

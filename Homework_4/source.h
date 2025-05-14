#ifndef SOURCE_H
#define SOURCE_H
#include <cstddef>
#include <bits/pthreadtypes.h>
// Cant write this with queue, queue is dynamic

inline class Memory {
public:
    int buff[10];
    pthread_mutex_t mutex;
    pthread_mutexattr_t attribute;
    std::size_t current_size;
} SourseMemory;

void print_buffer(int buff[], const std::size_t size) {
    for (std::size_t i = 0; i < size; i++) {
        std::cout << buff[i] << " ";
    }
    std::cout << std::endl;
}

inline void cleanup(Memory* header, bool destroy)
{
    if (header == nullptr)
        return;

    if (destroy) {
        printf("%s\n", "Destroy is requested.");

        // Destroying mutex attribute.
        if (pthread_mutexattr_destroy(&header->attribute) != 0)
            perror("Unable to destroy the mutex attribute");

        // Destroying mutex/
        if (pthread_mutex_destroy(&header->mutex) != 0)
            perror("Unable to destroy the mutex");

        // Getting shared memory.
        const int shm_id = shmget(0x2022005, sizeof(Memory), IPC_CREAT | 0660);
        if (shm_id < 0)
            perror("Unable to get shared memory ID for deletion");

        // Removing shared memory with IPC_RMID command.
        if (shmctl(shm_id, IPC_RMID, nullptr) != 0)
            perror("Unable to delete shared memory");
    }

    // Detaching shared memory.
    if (shmdt(header) != 0)
        perror("Unable to detach from the shared memory");
}

inline Memory* get_shared_mutex()
{


    // Calculating the size of header.
    const size_t size   = sizeof(Memory);

    // Determing the size. For example, it is year|your group number.
    constexpr key_t  key    = 0x2022005;

    // IPC_CREAT means to try to create or get already
    // existing shared memory. 0660 is the user permission.
    constexpr int flag   = IPC_CREAT | 0660;

    // Trying to get shared memory.
    // Here we are to trying to exclusively create shared
    // memory with IPC_EXCL which can fail if it
    // has already been created previously.
    int shm_id = shmget(key, size, flag | IPC_EXCL);
    if (shm_id < 0) {
        perror("Unable to create a new instance of shared memory");
        printf("%s\n", "Currently accessing the existing one...");

        // Trying to get already created shared memory.
        shm_id = shmget(key, size, flag);
        if (shm_id < 0) {
            perror("Unable to access the already created shared memory.\n");
            return nullptr;
        }
    }

    // Attaching to the shared memory.
    void* header = shmat(shm_id, NULL, 0);
    if (header == reinterpret_cast<void*>(-1)) {
        perror("Unable to attach to the shared memory");
        return nullptr;
    }

    return static_cast<Memory*>(header);
}

#endif //SOURCE_H
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "source.h"
#include <ctime>

int main() {

    //get shared memory set buff current size 0
    auto* shm = get_shared_mutex();
    SourseMemory.current_size = 0;

    srand(time(nullptr));

    const pid_t producer = fork();
    if (producer == -1) {
        perror("Error forking producer");
        return 1;
    }
    if (producer == 0) {
        for (int i = 0; i < 10; i++) {
            pthread_mutex_lock(&shm->mutex);

            if (shm->current_size < 10) {
                int elem = rand() % 100;
                shm->buff[shm->current_size] = elem;
                ++shm->current_size;
                std::cout << elem << " was produced: ";
                print_buffer(shm->buff, shm->current_size);
            }

            pthread_mutex_unlock(&shm->mutex);
            sleep(2);
        }
        exit(0);
    }

    const pid_t consumer = fork();
    if (consumer == -1) {
        perror("Error forking consumer");
        return 1;
    }
    if (consumer == 0) {
        for (int i = 0; i < 10; i++) {
            pthread_mutex_lock(&shm->mutex);

            if (shm->current_size > 0) {
                int consumed = shm->buff[shm->current_size - 1];
                --shm->current_size;
                std::cout << consumed << " was consumed: ";
                print_buffer(shm->buff, shm->current_size);
            }

            pthread_mutex_unlock(&shm->mutex);
            sleep(2);
        }
        exit(0);
    }

    wait(nullptr);
    wait(nullptr);

    cleanup(shm, true);
    return 0;
}

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <pthread.h>
#include <ctime>

int sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int random_seed(int thread_id) {
    unsigned int seed = time(nullptr) + thread_id;
    seed *= 37;
    seed %= 701;
    return seed;
}

void* generate_random(void* arg) {
    int thread_id = *static_cast<int*>(arg);

    struct random_data rand_state{};
    char state_buf_str[32];

    memset(&rand_state, 0, sizeof(rand_state));
    memset(state_buf_str, 0, sizeof(state_buf_str));

    initstate_r(random_seed(thread_id), state_buf_str, sizeof(state_buf_str), &rand_state);

    int32_t rand_num;
    random_r(&rand_state, &rand_num);

    rand_num = rand_num % 1000;

    pthread_mutex_lock(&mutex);
    sum += rand_num;
    pthread_mutex_unlock(&mutex);

    printf("Thread %d generated random number: %d\n", thread_id, rand_num);

    return nullptr;
}

int main() {

    pthread_t threads[5];
    int thread_id[5];

    for (int i = 0; i < 5; i++) {
        thread_id[i] = i + 1;
        pthread_create(&threads[i], nullptr, generate_random, &thread_id[i]);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], nullptr);
    }
    printf("Sum of all random numbers is %d\n", sum);

    pthread_mutex_destroy(&mutex);

    return 0;
}

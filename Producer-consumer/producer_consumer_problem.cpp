#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>

pthread_cond_t  full_;
pthread_cond_t  empty_;
pthread_mutex_t lock_;
std::deque<int> queue_;
size_t          size_; 

void print_queue(const std::deque<int>& queue) {
    if (queue.empty()) {
        std::cout << "queue is empty" << std::endl;
        return;
    }
    for (auto item : queue)
    {
        std::cout << item << ' ';
    }
    std::cout << std::endl;
}

void* execute_producer(void* input)
{
    for (size_t i = 0; i < 25UL; i++) {
        sleep(1);

        // item producing
        const int new_item = rand() % 999;
        
        // Enter critical section
        pthread_mutex_lock(&lock_);

        // we need while for spurious wake-ups
        while (queue_.size() == size_)
        {
            pthread_cond_wait(&empty_, &lock_);
        }

        std::cout << "New item " << new_item << " is produced and inserted into the storage: ";

        queue_.push_back(new_item);
        print_queue(queue_);

        // Leave critical section
        pthread_cond_signal(&full_);
        pthread_mutex_unlock(&lock_);
    }

    return nullptr;
}

void* execute_consumer(void* input)
{
    for (size_t i = 0; i < 25UL; i++) {
        sleep(1);

        // Enter critical section
        pthread_mutex_lock(&lock_);
        while (queue_.empty())
        {
            pthread_cond_wait(&full_, &lock_);
        }
        
        const int item = queue_.front();
        
        std::cout << "New item " << item << " is consumed and removed from the storage: ";
        queue_.pop_front();
        print_queue(queue_);

        // Leave critical section
        pthread_cond_signal(&empty_);
        pthread_mutex_unlock(&lock_);
    }

    return nullptr;
}


int main(int argc, char** argv)
{
    if (argc != 4) {
        stderr;
        return 1;
    }   

    // Parse command line arguments
    // TODO
    size_t q_size = atoi(argv[1]);
    size_t prod_size = atoi(argv[2]);
    size_t cons_size = atoi(argv[3]);

    // Initialize synchronization context
    pthread_mutex_init(&lock_, nullptr);
    pthread_cond_init(&full_,  nullptr);
    pthread_cond_init(&empty_, nullptr);
    size_ = q_size;

    pthread_t consumers[cons_size];
    for (int i = 0; i < cons_size; ++i)
    {
        pthread_create(&consumers[i], nullptr, execute_consumer, nullptr);
    }
    
    pthread_t prod[prod_size];
    for (int i = 0; i < prod_size; ++i)
    {
        pthread_create(&prod[i], nullptr, execute_producer, nullptr);
    }
     
    // Join producer threads
    for (int i = 0; i < cons_size; ++i)
    {
        pthread_join(consumers[i], nullptr);
    }

    // Join consumer threads
    for (int i = 0; i < prod_size; ++i)
    {
        pthread_join(prod[i], nullptr);
    }

    // Destroy synchronization context
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&empty_);
    pthread_cond_destroy(&full_);

    return 0;
}

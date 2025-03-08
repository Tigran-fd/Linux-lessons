#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct SharedData {
    int fd;
    bool writer_active;
};

void* execute_reader(void* input)
{
    const auto* data = static_cast<SharedData *>(input);

    for (size_t i = 0; i < 25UL; i++) {
        sleep(3);

        pthread_mutex_lock(&lock);
        while (data->writer_active) {
            pthread_cond_wait(&reader_cond, &lock);
        }
        pthread_mutex_unlock(&lock);

        lseek(data->fd, 0, SEEK_SET);
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = read(data->fd, buffer, BUFFER_SIZE - 1);
        if (bytesRead == -1) {
            perror("Error reading file");
        } else {
            buffer[bytesRead] = '\0';
            printf("Reader Thread: File content:\n%s\n", buffer);
        }
    }
    return nullptr;
}

void* execute_writer(void* input)
{
    auto* data = static_cast<SharedData *>(input);

    for (size_t i = 0; i < 25UL; i++) {
        sleep(3);

        pthread_mutex_lock(&lock);
        while (data->writer_active) {
            pthread_cond_wait(&writer_cond, &lock);
        }
        data->writer_active = true;
        pthread_mutex_unlock(&lock);

        constexpr char writeText[] = "homework is ready!.\n";
        ssize_t bytesWritten = write(data->fd, writeText, sizeof(writeText) - 1);
        if (bytesWritten == -1) {
            perror("Error writing to file");
        } else {
            printf("Writer Thread: Wrote to file.\n");
        }

        pthread_mutex_lock(&lock);
        data->writer_active = false;
        pthread_cond_signal(&reader_cond);
        pthread_cond_signal(&writer_cond);
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <readers_count> <writers_count>" << std::endl;
        return 1;
    }

    size_t readers_count = std::atoi(argv[1]);
    size_t writers_count = std::atoi(argv[2]);

    SharedData data{};
    data.fd = open("file.txt", O_RDWR | O_CREAT, 0644);
    if (data.fd == -1) {
        perror("Error opening file");
        return 1;
    }
    data.writer_active = false;

    pthread_t readers[readers_count];
    pthread_t writers[writers_count];

    for (size_t i = 0; i < readers_count; ++i) {
        pthread_create(&readers[i], nullptr, execute_reader, &data);
    }

    for (size_t i = 0; i < writers_count; ++i) {
        pthread_create(&writers[i], nullptr, execute_writer, &data);
    }

    for (size_t i = 0; i < readers_count; ++i) {
        pthread_join(readers[i], nullptr);
    }

    for (size_t i = 0; i < writers_count; ++i) {
        pthread_join(writers[i], nullptr);
    }

    close(data.fd);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&reader_cond);
    pthread_cond_destroy(&writer_cond);
    return 0;
}

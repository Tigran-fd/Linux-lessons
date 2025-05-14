#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

struct sockaddr_in host_addr, peer_addr;

[[noreturn]] void *receive(void *arg) {
    auto sockfd = static_cast<int*>(arg);
    char buffer[BUFFER_SIZE];
    while (true) {
        int n = recvfrom(*sockfd, buffer, BUFFER_SIZE - 1, 0, nullptr, nullptr);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s\n", buffer);
        }
    }
}

[[noreturn]] void *send(void *arg) {
    auto sockfd = static_cast<int*>(arg);
    char buffer[BUFFER_SIZE];
    while (true) {
        if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strcmp(buffer, "bye!") == 0 || strcmp(buffer, "good bye!") == 0) {
                sendto(*sockfd, buffer, strlen(buffer), 0, reinterpret_cast<struct sockaddr*>(&peer_addr), sizeof(peer_addr));
                printf("Chat is over.\n");
                exit(0);
            }
            sendto(*sockfd, buffer, strlen(buffer), 0, reinterpret_cast<struct sockaddr*>(&peer_addr), sizeof(peer_addr));
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host_port> <peer_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sockfd;

    const int host_port = atoi(argv[1]);
    const int peer_port = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&host_addr, 0, sizeof(host_addr));
    memset(&peer_addr, 0, sizeof(peer_addr));

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = INADDR_ANY;
    host_addr.sin_port = htons(host_port);

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = INADDR_ANY;
    peer_addr.sin_port = htons(peer_port);

    if (bind(sockfd, reinterpret_cast<struct sockaddr*>(&host_addr), sizeof(host_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, nullptr, receive, &sockfd);
    pthread_create(&send_thread, nullptr, send, &sockfd);

    pthread_join(recv_thread, nullptr);
    pthread_join(send_thread, nullptr);

    close(sockfd);
    return 0;
}

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "Tic-Tac-Toe.h"
#define SHM_NAME "game_memory"
#define BOARD_SIZE 3

void draw_board(const SharedGame *game)
{
    std::cout << "\n";
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (game->board[i][j]) std::cout << game->board[i][j];
            else std::cout << " ";
            if (j<BOARD_SIZE - 1) std::cout << " | ";
            else std::cout << "";
        }
        std::cout << std::endl;
        if (i < BOARD_SIZE - 1)
            std::cout << "---------"<< std::endl;
    }
    std::cout << std::endl;
}

bool win(const SharedGame *game,char symbol)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if ((game->board[i][0] == symbol && game->board[i][1] == symbol && game->board[i][2] == symbol) ||
            (game->board[0][i] == symbol && game->board[1][i] == symbol && game->board[2][i] == symbol))
            return true;
    }
    if ((game->board[0][0] == symbol && game->board[1][1] == symbol && game->board[2][2] == symbol) ||
        (game->board[0][2] == symbol && game->board[1][1] == symbol && game->board[2][0] == symbol))
        return true;
    return false;
}

SharedGame* initialize_shared_memory(int &shm_fd) {
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_fd, sizeof(SharedGame));
    auto* game = static_cast<SharedGame*>(mmap(nullptr, sizeof(SharedGame), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (game == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    return game;
}

void initialize_sync_primitives(SharedGame* game) {
    memset(game, 0, sizeof(SharedGame));
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;
    pthread_mutexattr_init(&mattr);
    pthread_condattr_init(&cattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&game->mutex, &mattr);
    pthread_cond_init(&game->cond, &cattr);
    game->current_turn = 1;
}

void wait_for_turn(SharedGame* game, int player) {
    while (!game->game_over && game->current_turn != player)
        pthread_cond_wait(&game->cond, &game->mutex);
}

void print_result(const SharedGame* game) {
    draw_board(game);
    if (game->winner == '-') {
        std::cout << "It's a draw!\n";
    } else {
        std::cout << "Player " << game->winner << " wins!\n";
    }
}

void cleanup(SharedGame* game,int player,int shm_fd) {
    if (player == 1) {
        pthread_mutex_destroy(&game->mutex);
        pthread_cond_destroy(&game->cond);
        shm_unlink(SHM_NAME);
    }
    munmap(game, sizeof(SharedGame));
    close(shm_fd);
}
void play_game(const char* role) {
    int shm_fd;
    SharedGame* game = initialize_shared_memory(shm_fd);

    int player = strcmp(role, "player1") == 0 ? 1 : 2;
    char symbol = player == 1 ? 'X' : 'O';

    if (player == 1) initialize_sync_primitives(game);

    while (true) {
        pthread_mutex_lock(&game->mutex);
        wait_for_turn(game, player);

        if (game->game_over) {
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        draw_board(game);
        std::cout << "Player " << player << " (" << symbol << "), enter your move (row col): " << std::flush;
        int row, col;
        std::cin >> row >> col;

        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || game->board[row][col]) {
            std::cout << "Invalid move!\n";
            pthread_mutex_unlock(&game->mutex);
            continue;
        }

        game->board[row][col] = symbol;
        game->move_count++;

        if (win(game, symbol)) {
            game->game_over = 1;
            game->winner = symbol;
        } else if (game->move_count == 9) {
            game->game_over = 1;
            game->winner = '-';
        } else {
            game->current_turn = (player == 1) ? 2 : 1;
        }

        draw_board(game);

        pthread_cond_signal(&game->cond);
        pthread_mutex_unlock(&game->mutex);

        if (game->game_over) break;
    }

    pthread_mutex_lock(&game->mutex);
    print_result(game);
    pthread_mutex_unlock(&game->mutex);

    if (player == 2) sleep(1);
    cleanup(game, player, shm_fd);
}
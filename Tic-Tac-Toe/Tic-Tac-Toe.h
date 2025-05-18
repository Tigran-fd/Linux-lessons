#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H
#include <pthread.h>
#define SHM_NAME "game_memory"
#define BOARD_SIZE 3


typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE]; // Game board: 'X', 'O', or ''
    int current_turn; // 1 for player1, 2 for player2
    int move_count;
    int game_over; // 1 when game ends
    char winner;   // 'X', 'O', or '-' for draw
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SharedGame;

void draw_board(const SharedGame *game);

bool win(const SharedGame *game,char symbol);

SharedGame* initialize_shared_memory(int &shm_fd);

void initialize_sync_primitives(SharedGame* game);

void wait_for_turn(SharedGame* game, int player);

void print_result(const SharedGame* game);

void cleanup(SharedGame* game,int player,int shm_fd);

void play_game(const char* role);
#endif //TIC_TAC_TOE_H

#include "Tic-Tac-Toe.h"
#include <cstring>
#include <iostream>

int main(const int argc, char *argv[]) {
        if (argc != 2 || (strcmp(argv[1], "player1") != 0 && strcmp(argv[1], "player2") != 0)) {
            std::cerr << "Usage: " << argv[0] << " player1|player2\n";
            return 1;
        }
        play_game(argv[1]);
        return 0;
}
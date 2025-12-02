#include <stdio.h>
#include <time.h>    // for seeding the random number generator
#include "plantamitz.h"    // include the game header to use GameState and game functions

int main(void) {
    // Seed the random number generator for board initialization
    srand((unsigned) time(NULL));

    // Create a GameState instance to hold the game state
    GameState game;

    // Initialize the game (setup board, contract, etc.)
    initGame(&game);

    // Run the main game loop
    gameLoop(&game);

    // After the loop ends, determine if the player won or lost and display the appropriate message.
    if (isGameWon(&game)) {
        printf("Congratulations, you fulfilled the contract and won the game!\n");
    } else if (isGameOver(&game)) {
        printf("Game Over. You ran out of moves before fulfilling the contract.\n");
    }

    return 0;
}

#ifndef PLANTAMITZ_H
#define PLANTAMITZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// Include Windows API for console functions (like colors, cursor control) if on Windows
#ifdef _WIN32
#include <windows.h>
#endif

// Define board dimensions
#define BOARD_ROWS 25
#define BOARD_COLS 45
typedef struct GameState {
    int score;
    int level;
    // Add other game state variables as needed
} GameState;

// Define the GameState structure holding the game board and state variables.
typedef struct {
    char board[BOARD_ROWS][BOARD_COLS];  // The game board represented as a 25x45 grid of characters
    int movesRemaining;                 // Moves remaining for the player
    int targetCount;                    // Total items to collect to fulfill the contract (simplified goal)
    int collectedCount;                 // Total items collected so far towards the contract
    // Future extension: We could add detailed contract targets for each item type and track collected counts per type.
} GameState;

// Initialize the game state (set up board, contract, moves, etc.)
void initGame(GameState *game);

// Run the main game loop until the contract is fulfilled or moves are exhausted.
void gameLoop(GameState *game);

// Check if the game has been won (contract fulfilled).
bool isGameWon(const GameState *game);

// Check if the game is over (no moves remaining and contract not fulfilled).
bool isGameOver(const GameState *game);

#endif // PLANTAMITZ_H

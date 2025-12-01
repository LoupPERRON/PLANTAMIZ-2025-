#include <stdio.h>

#define ROWS 25
#define COLS 46

int main() {
    int table[ROWS][COLS];
    
    // Initialize table with zeros
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            table[i][j] = 0;
        }
    }
    
    // Example: print table dimensions
    printf("Table created: %d rows x %d columns\n", ROWS, COLS);
    
    return 0;
}
#pragma once

#include <stdbool.h>

#define ROWS 10
#define COLS 10

typedef struct {
    char cells[ROWS][COLS];
} Board;

void board_init(Board *b);
void board_print(Board *b, int cursor_r, int cursor_c, int selected_r, int selected_c);
void board_apply_gravity(Board *b);
bool board_find_and_remove_matches(Board *b, int *points);
void board_swap(Board *b, int r1,int c1,int r2,int c2);
bool board_no_initial_matches(Board *b);
int board_count_char(Board *b,char ch);

#pragma once

#include <stdbool.h> // Pour le type bool

#define ROWS 10 // Nombre de lignes du plateau
#define COLS 10 // Nombre de colonnes du plateau

typedef struct {
    char cells[ROWS][COLS]; // Grille du plateau
} Board; // Structure représentant le plateau de jeu

void board_init(Board *b); // Initialise le plateau avec des caractères aléatoires
void board_print(Board *b, int cursor_r, int cursor_c, int selected_r, int selected_c); // Affiche le plateau avec le curseur et la sélection
void board_apply_gravity(Board *b); // Applique la gravité pour faire tomber les caractères
bool board_find_and_remove_matches(Board *b, int *points); // Trouve et supprime les correspondances, retourne vrai si des correspondances ont été trouvées
void board_swap(Board *b, int r1,int c1,int r2,int c2); // Échange deux cellules du plateau
bool board_no_initial_matches(Board *b); // Vérifie qu'il n'y a pas de correspondances initiales sur le plateau
int board_count_char(Board *b,char ch); // Compte le nombre d'occurrences d'un caractère donné sur le plateau

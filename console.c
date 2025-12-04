#include "console.h"

void gotoligcol(int ligne, int colonne) {
    COORD position;
    position.X = colonne;
    position.Y = ligne;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void Color(int couleurTexte, int couleurFond) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, couleurFond * 16 + couleurTexte);
}

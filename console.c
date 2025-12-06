#include <windows.h>
#include <stdio.h>
#include "console.h"

// Déplace le curseur à la position (lig, col) [cite: 196]
void gotoligcol(int lig, int col) {
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
}

// Change la couleur du texte et du fond [cite: 231]
void Color(int couleurDuTexte, int couleurDeFond) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
}

// Fonction utilitaire pour nettoyer la console
void effacerEcran() {
    system("cls");
}

// Cache le curseur clignotant pour une meilleure esthétique
void cacherCurseur() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
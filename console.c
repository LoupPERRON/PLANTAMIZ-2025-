#include "console.h"
#include <stdio.h>

void gotoligcol(int lig,int col)
{
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
}

void Color(int couleurDuTexte,int couleurDeFond)
{
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond*16 + couleurDuTexte);
}

void clear_screen()
{
    system("cls");
}

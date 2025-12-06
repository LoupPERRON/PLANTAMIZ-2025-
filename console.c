#include "console.h"
#include <stdio.h>

void gotoligcol(int lig,int col) // Déplace le curseur à la ligne 'lig' et la colonne 'col'
{
    COORD mycoord; //   Déclaration d'une variable de type COORD pour stocker les coordonnées
    mycoord.X = col; //   Définir la position X (colonne)
    mycoord.Y = lig; //   Définir la position Y (ligne)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord); //   Déplacer le curseur à la position spécifiée
}

void Color(int couleurDuTexte,int couleurDeFond) // Change la couleur du texte et du fond
{
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE); // Obtenir le handle de la console standard
    SetConsoleTextAttribute(H, couleurDeFond*16 + couleurDuTexte); // Définir les attributs de texte (couleur du texte et du fond)
}

void clear_screen() // Efface l'écran du console
{
    system("cls"); // Utiliser la commande système 'cls' pour effacer l'écran de la console
}

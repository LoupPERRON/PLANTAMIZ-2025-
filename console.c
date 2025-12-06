#include "console.h"
#include <stdio.h>

void gotoligcol(int lig,int col) // Déplace le curseur à la ligne 'lig' et la colonne 'col'
{
    COORD mescoord; //   Déclaration d'une variable de type COORD pour stocker les coordonnées
    mescoord.X = col; //   Définir la position X (colonne)
    mescoord.Y = lig; //   Définir la position Y (ligne)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mescoord); //   Déplacer le curseur à la position spécifiée
}

void Couleur(int couleurDuTexte,int couleurDeFond) // Change la couleur du texte et du fond
{
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE); // Obtenir le handle de la console standard
    SetConsoleTextAttribute(H, couleurDeFond*16 + couleurDuTexte); // Définir les attributs de texte (couleur du texte et du fond)
}

void effacer_ecran() // Efface l'écran du console
{
    system("cls"); // Utiliser la commande système 'cls' pour effacer l'écran
}

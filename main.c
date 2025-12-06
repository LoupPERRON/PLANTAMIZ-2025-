#include <stdio.h> // Pour les fonctions d'entrée/sortie standard
#include <string.h> // Pour les fonctions de manipulation de chaînes
#include "game.h" // Inclure le fichier d'en-tête pour les fonctions de jeu

static int menu() // Affiche le menu et retourne le choix de l'utilisateur
{
    while(1){
        printf("Plantamitz - Menu\n"); // afficher le menu
        printf("1) Nouvelle partie\n"); // afficher nouvelle partie
        printf("2) Reprendre\n"); // afficher reprendre
        printf("3) Quitter\n"); // afficher quitter
        printf("Choix: "); // demander le choix
        int c = getchar(); // lire le choix
        while(getchar()!='\n' && !feof(stdin)); // vider le buffer
        if(c=='1') return 1; // nouvelle partie
        if(c=='2') return 2; // reprendre partie
        if(c=='3') return 3; // quitter
        printf("Choix invalide.\n"); // choix invalide
    }
}

int main()
{
    int choice = menu(); // afficher le menu et obtenir le choix de l'utilisateur
    if(choice==1){
        game_run(0); // nouvelle partie
    } else if(choice==2){
        game_run(1); // reprendre partie
    } else {
        printf("Au revoir.\n"); // quitter
    }
    return 0; // fin du programme
}

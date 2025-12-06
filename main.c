#include <stdio.h>
#include <stdlib.h>
#include <time.h>#include "plantamitz.h"

int main() {
    // Initialisation du générateur aléatoire 
    srand(time(NULL));

    // Lancement du jeu
    lancerJeu();

    return 0;
}
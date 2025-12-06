#ifndef INPUT_H
#define INPUT_H

typedef enum {
    HAUT, BAS, GAUCHE, DROITE, SELECTION, QUITTER, RIEN
} Action;

// Récupère l'action du joueur (bloquant)
Action recupererAction();

#endif
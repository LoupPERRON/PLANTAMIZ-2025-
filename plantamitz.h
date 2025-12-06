#ifndef PLANTAMITZ_H
#define PLANTAMITZ_H

#include "tableau.h"

typedef struct {
    int niveau;
    int score;
    int coupsRestants;
    int vies;
    int objectifScore; // Objectif simplifié pour l'exemple
} GameState;

void lancerJeu();
void jouerNiveau(GameState *etat);

#endif
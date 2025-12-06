#ifndef AFFICHER_H
#define AFFICHER_H

#include "tableau.h"
#include "plantamitz.h"

void afficherJeu(Plateau *p, GameState *etat, int curseurL, int curseurC);
void afficherMenu();
void afficherFin(int gagne, int score);

#endif
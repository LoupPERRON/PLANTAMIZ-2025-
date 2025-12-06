#ifndef CONSOLE_H
#define CONSOLE_H

// Prototypes pour la gestion de la console Windows
// Basé sur les annexes 2 et 3 du sujet [cite: 194, 215]

void gotoligcol(int lig, int col);
void Color(int couleurDuTexte, int couleurDeFond);
void effacerEcran();
void cacherCurseur();

#endif
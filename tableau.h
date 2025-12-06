#ifndef TABLEAU_H
#define TABLEAU_H

// Dimensions du plateau 
#define LIGNES 25
#define COLONNES 45

// Structure représentant une case
typedef struct {
    char type;          // 'S', 'F', 'P', 'O', 'M'
    int estSelectionne; // 1 si l'item est sélectionné par le joueur
    int aSupprimer;     // Flag pour marquer les items à détruire
} Case;

// Structure du plateau
typedef struct {
    Case grille[LIGNES][COLONNES];
} Plateau;

// Prototypes
void initialiserPlateau(Plateau *p);
void remplirPlateau(Plateau *p);
int verifierMatchs(Plateau *p, int *score);
void appliquerGravite(Plateau *p);
void permuterItems(Plateau *p, int l1, int c1, int l2, int c2);
char genererItemAleatoire();
int estMouvementValide(int l, int c);

#endif
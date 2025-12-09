#pragma once

#include <stdbool.h> // Pour le type bool

#define LIGNES 10 // Nombre de lignes du plateau
#define COLONNES 10 // Nombre de colonnes du plateau

typedef struct {
    char cellules[LIGNES][COLONNES]; // Grille du plateau
} Tableau; // Structure représentant le plateau de jeu

void init_tableau(Tableau *b); // Initialise le plateau avec des caractères aléatoires
void Tableau_print(Tableau *b, int curseur_l, int curseur_c, int choisi_l, int choisi_c); // Affiche le plateau avec le curseur et la sélection
void Tableau_appliquer_gravité(Tableau *b); // Applique la gravité pour faire tomber les caractères
// Trouve et supprime les correspondances, retourne vrai si des correspondances ont été trouvées
bool Tableau_trouver_et_supprimer_les_correspondances(Tableau *b, int *points, int *removed_counts,
                                                      int delai_ms, int curseur_l, int curseur_c, int choisi_l, int choisi_c,
                                                      int niveau, int coups_restants, int vies, int points_total,
                                                      int progres[5], int targets[5]);
void Tableau_swap(Tableau *b, int r1, int c1, int r2, int c2); // Échange deux cellules du plateau
bool Tableau_pas_de_correspondances_initiales(Tableau *b); // Vérifie qu'il n'y a pas de correspondances initiales sur le plateau
int Tableau_count_char(Tableau *b, char ch); // Compte le nombre d'occurrences d'un caractère donné sur le plateau

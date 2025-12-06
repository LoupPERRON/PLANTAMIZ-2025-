#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "tableau.h"

// Items disponibles : Soleil, Fraise, Pomme, Oignon, Mandarine 
const char ITEMS[] = {'S', 'F', 'P', 'O', 'M'};

char genererItemAleatoire() {
    return ITEMS[rand() % 5];
}

// Initialise le plateau et s'assure qu'il n'y a pas de match au départ [cite: 28]
void initialiserPlateau(Plateau *p) {
    int scoreTemp = 0;
    do {
        remplirPlateau(p);
    } while (verifierMatchs(p, &scoreTemp)); // Recommence tant qu'il y a des matchs initiaux
    
    // Reset des flags de suppression après l'initialisation propre
    for(int i=0; i<LIGNES; i++) {
        for(int j=0; j<COLONNES; j++) {
            p->grille[i][j].aSupprimer = 0;
            p->grille[i][j].estSelectionne = 0;
        }
    }
}

void remplirPlateau(Plateau *p) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            // Remplit seulement les cases vides (utilisé aussi après gravité) [cite: 32]
            if (p->grille[i][j].type == ' ' || p->grille[i][j].type == 0) {
                 p->grille[i][j].type = genererItemAleatoire();
                 p->grille[i][j].aSupprimer = 0;
                 p->grille[i][j].estSelectionne = 0;
            }
        }
    }
}

// Permute deux items dans la grille [cite: 37]
void permuterItems(Plateau *p, int l1, int c1, int l2, int c2) {
    Case temp = p->grille[l1][c1];
    p->grille[l1][c1] = p->grille[l2][c2];
    p->grille[l2][c2] = temp;
    
    // Réinitialise la sélection après permutation
    p->grille[l1][c1].estSelectionne = 0;
    p->grille[l2][c2].estSelectionne = 0;
}

// Vérifie les alignements de 3 items ou plus [cite: 29]
// Note: Simplifié ici pour les alignements H/V. Les formes complexes (H, Rectangle) 
// nécessiteraient une logique de détection de motifs plus avancée.
int verifierMatchs(Plateau *p, int *score) {
    int matchTrouve = 0;
    
    // Reset flags
    for(int i=0; i<LIGNES; i++)
        for(int j=0; j<COLONNES; j++)
            p->grille[i][j].aSupprimer = 0;

    // Vérification Horizontale
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES - 2; j++) {
            char type = p->grille[i][j].type;
            if (type != ' ' && 
                p->grille[i][j+1].type == type && 
                p->grille[i][j+2].type == type) {
                
                p->grille[i][j].aSupprimer = 1;
                p->grille[i][j+1].aSupprimer = 1;
                p->grille[i][j+2].aSupprimer = 1;
                matchTrouve = 1;
                *score += 3; // Points de base
            }
        }
    }

    // Vérification Verticale
    for (int j = 0; j < COLONNES; j++) {
        for (int i = 0; i < LIGNES - 2; i++) {
            char type = p->grille[i][j].type;
            if (type != ' ' && 
                p->grille[i+1][j].type == type && 
                p->grille[i+2][j].type == type) {
                
                p->grille[i][j].aSupprimer = 1;
                p->grille[i+1][j].aSupprimer = 1;
                p->grille[i+2][j].aSupprimer = 1;
                matchTrouve = 1;
                *score += 3;
            }
        }
    }
    
    return matchTrouve;
}

// Fait tomber les items pour combler les trous (Gravité) 
void appliquerGravite(Plateau *p) {
    // 1. Suppression
    for(int i=0; i<LIGNES; i++) {
        for(int j=0; j<COLONNES; j++) {
            if(p->grille[i][j].aSupprimer) {
                p->grille[i][j].type = ' '; // Devient un trou
                p->grille[i][j].aSupprimer = 0;
            }
        }
    }

    // 2. Gravité (colonne par colonne, de bas en haut)
    for (int j = 0; j < COLONNES; j++) {
        int ecriture = LIGNES - 1;
        for (int lecture = LIGNES - 1; lecture >= 0; lecture--) {
            if (p->grille[lecture][j].type != ' ') {
                p->grille[ecriture][j] = p->grille[lecture][j];
                ecriture--;
            }
        }
        // Remplir le haut avec du vide
        while (ecriture >= 0) {
            p->grille[ecriture][j].type = ' ';
            ecriture--;
        }
    }

    // 3. Remplissage des nouveaux trous par le haut [cite: 32]
    remplirPlateau(p);
}

int estMouvementValide(int l, int c) {
    return (l >= 0 && l < LIGNES && c >= 0 && c < COLONNES);
}
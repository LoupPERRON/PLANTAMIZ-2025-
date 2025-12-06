#include <stdio.h>
#include "afficher.h"
#include "console.h"

// Codes couleurs basés sur l'Annexe 3 [cite: 257]
#define C_FRAISE 12     // Rouge fluo
#define C_SOLEIL 14     // Jaune
#define C_POMME 10      // Vert fluo
#define C_OIGNON 15     // Blanc
#define C_MANDARINE 13  // Violet (pour différencier)
#define C_FOND 0        // Noir

void afficherJeu(Plateau *p, GameState *etat, int curseurL, int curseurC) {
    gotoligcol(0, 0); // Retour au début pour redessiner sans clignotement

    printf("=== PLANTAMITZ 2025 ===\n"); // [cite: 3]
    printf("Score: %d | Coups restants: %d | Vies: %d\n", etat->score, etat->coupsRestants, etat->vies);
    printf("Objectif: %d points\n\n", etat->objectifScore); // Simplification du contrat pour l'exemple

    // Affichage de la grille
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            
            int couleur = 7; // Gris par défaut
            char c = p->grille[i][j].type;

            switch(c) {
                case 'F': couleur = C_FRAISE; break;
                case 'S': couleur = C_SOLEIL; break;
                case 'P': couleur = C_POMME; break;
                case 'O': couleur = C_OIGNON; break;
                case 'M': couleur = C_MANDARINE; break;
            }

            // Gestion surbrillance curseur et sélection [cite: 43]
            int fond = C_FOND;
            if (i == curseurL && j == curseurC) {
                fond = 8; // Gris foncé pour le curseur
            }
            if (p->grille[i][j].estSelectionne) {
                fond = 1; // Bleu foncé si sélectionné
                c = tolower(c); // Minuscule si sélectionné [cite: 43]
            }

            Color(couleur, fond);
            printf("%c", c);
        }
        Color(15, 0); // Reset fin de ligne
        printf("\n");
    }
}

void afficherMenu() {
    effacerEcran();
    Color(10, 0);
    printf("Bienvenue dans PLANTAMITZ 2025\n");
    printf("Appuyez sur une touche pour commencer...\n");
    Color(15, 0);
}

void afficherFin(int gagne, int score) {
    effacerEcran();
    if (gagne) {
        Color(14, 0);
        printf("BRAVO ! Niveau reussi ! Score final: %d\n", score);
    } else {
        Color(12, 0);
        printf("ECHEC... Plus de coups.\n");
    }
    Color(15, 0);
    printf("Appuyez pour continuer...");
}
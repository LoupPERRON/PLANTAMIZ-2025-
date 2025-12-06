#include <stdio.h>
#include <conio.h>
#include "plantamitz.h"
#include "tableau.h"
#include "input.h"
#include "afficher.h"
#include "console.h"

void lancerJeu() {
    afficherMenu();
    _getch();

    GameState etat;
    etat.vies = 5; // [cite: 73]
    etat.niveau = 1;

    while (etat.vies > 0 && etat.niveau <= 3) {
        // Configuration du niveau
        etat.score = 0;
        etat.coupsRestants = 30 + (etat.niveau * 5); // Difficulté croissante
        etat.objectifScore = 50 * etat.niveau;
        
        jouerNiveau(&etat);

        if (etat.score >= etat.objectifScore) {
            afficherFin(1, etat.score);
            etat.niveau++;
            etat.vies = 5; // Reset vies si niveau réussi [cite: 75]
        } else {
            afficherFin(0, etat.score);
            etat.vies--;
        }
        _getch();
    }
}

void jouerNiveau(GameState *etat) {
    Plateau plateau;
    initialiserPlateau(&plateau); // [cite: 27, 28]

    int curseurL = LIGNES / 2;
    int curseurC = COLONNES / 2;
    int selectionActive = 0;
    int selL = -1, selC = -1;
    int jeuEnCours = 1;

    cacherCurseur();

    while (jeuEnCours && etat->coupsRestants > 0 && etat->score < etat->objectifScore) {
        afficherJeu(&plateau, etat, curseurL, curseurC);
        
        Action act = recupererAction();

        switch (act) {
            case HAUT:   if (curseurL > 0) curseurL--; break;
            case BAS:    if (curseurL < LIGNES - 1) curseurL++; break;
            case GAUCHE: if (curseurC > 0) curseurC--; break;
            case DROITE: if (curseurC < COLONNES - 1) curseurC++; break;
            
            case SELECTION: // 
                if (!selectionActive) {
                    // Première sélection
                    selectionActive = 1;
                    selL = curseurL;
                    selC = curseurC;
                    plateau.grille[selL][selC].estSelectionne = 1; // [cite: 43]
                } else {
                    // Tentative de permutation
                    // Vérifier contiguïté (Haut, Bas, Gauche, Droite)
                    if (((curseurL == selL + 1 || curseurL == selL - 1) && curseurC == selC) ||
                        ((curseurC == selC + 1 || curseurC == selC - 1) && curseurL == selL)) {
                        
                        permuterItems(&plateau, selL, selC, curseurL, curseurC); // [cite: 44]
                        
                        int points = 0;
                        if (verifierMatchs(&plateau, &points)) {
                            // Match valide : appliquer suppressions et gravité
                            etat->score += points;
                            do {
                                // Boucle de stabilisation (réaction en chaîne) [cite: 33]
                                afficherJeu(&plateau, etat, -1, -1); // Animation
                                Sleep(300); // Pause visuelle
                                appliquerGravite(&plateau);
                                points = 0;
                            } while (verifierMatchs(&plateau, &points));
                            
                            etat->coupsRestants--;
                        } else {
                            // Mouvement invalide : on remet en place (si règle stricte)
                            // Le sujet dit : "Si la permutation ne forme aucun groupe... permuter de nouveau" [cite: 46]
                            // Ici on accepte la permutation sans match mais ça coûte un coup ou non selon interprétation.
                            // Dans CandyCrush classique, on annule. Le sujet dit [cite: 47] on peut permuter items isolés.
                            // On décrémente donc le coup.
                            etat->coupsRestants--;
                        }
                    }
                    
                    // Reset sélection
                    plateau.grille[selL][selC].estSelectionne = 0;
                    selectionActive = 0;
                }
                break;

            case QUITTER:
                jeuEnCours = 0;
                etat->vies = 0; // Force la sortie
                break;
            default: break;
        }
    }
}
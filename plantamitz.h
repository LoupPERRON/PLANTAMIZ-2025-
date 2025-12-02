#ifndef PLANTAMITZ_H
#define PLANTAMITZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Inclure l'API Windows pour les fonctions de la console (couleurs, position du curseur) si sous Windows
#ifdef _WIN32
#include <windows.h>
#endif

// Définir les dimensions du plateau de jeu
#define NB_LIGNES 25
#define NB_COLONNES 45

// Définition de la structure représentant l'état du jeu (plateau + informations)
typedef struct {
    char plateau[NB_LIGNES][NB_COLONNES];  // Matrice de 25x45 caractères représentant les items
    int coupsRestants;                     // Nombre de coups restants pour le joueur
    int objectifTotal;                     // Nombre total d’items à collecter pour remplir le contrat
    int itemsCollectes;                    // Nombre d’items déjà collectés par le joueur
    // Extension possible : objectifs détaillés par type d'item et suivi par type
} EtatJeu;

// Initialise l’état du jeu (plateau, contrat, compteurs, etc.)
void initialiserJeu(EtatJeu *jeu);

// Lance la boucle principale du jeu jusqu’à réussite du contrat ou épuisement des coups
void boucleJeu(EtatJeu *jeu);

// Vérifie si le contrat a été rempli (victoire)
bool jeuGagne(const EtatJeu *jeu);

// Vérifie si la partie est terminée sans avoir rempli le contrat (défaite)
bool jeuTermine(const EtatJeu *jeu);

#endif // PLANTAMITZ_H

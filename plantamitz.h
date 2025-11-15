#ifndef PLANTAMITZ_H
#define PLANTAMITZ_H

// Taille du plateau (CDC)
#define NB_LIGNES   25
#define NB_COLONNES 45

#define VIDE ' '              // case vide après suppression
#define NB_TYPES_ITEM 5       // S, F, P, O, M

// Codes des items (CDC)
// S = soleil, F = fraise, P = pomme, O = oignon, M = mandarine
#define ITEM_SOLEIL    'S'
#define ITEM_FRAISE    'F'
#define ITEM_POMME     'P'
#define ITEM_OIGNON    'O'
#define ITEM_MANDARINE 'M'

/******************************************************
 * Nom : initialiserAleatoire
 * Rôle : initialise la graine du générateur aléatoire
 * Paramètres : aucun
 * Résultat : aucun
 ******************************************************/
void initialiserAleatoire(void);

/******************************************************
 * Nom : genererItemAleatoire
 * Rôle : retourne un item aléatoire parmi S,F,P,O,M
 * Paramètres : aucun
 * Résultat : un caractère représentant l’item
 ******************************************************/
char genererItemAleatoire(void);

/******************************************************
 * Nom : initialiserGrille
 * Rôle : remplit la grille avec des items aléatoires
 *        puis la stabilise (plus aucun groupe de 3+)
 * Paramètres :
 *   grille (IN/OUT) : matrice 25x45 du plateau
 * Résultat : aucun
 ******************************************************/
void initialiserGrille(char grille[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : afficherGrille
 * Rôle : affiche la grille avec des couleurs
 * Paramètres :
 *   grille (IN) : matrice 25x45 du plateau
 * Résultat : aucun
 ******************************************************/
void afficherGrille(const char grille[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : detecterGroupesSimples
 * Rôle : détecte les groupes horizontaux/verticaux
 *        de 3 items ou plus (même nature)
 * Paramètres :
 *   grille (IN)  : plateau
 *   marque (OUT) : matrice d'entiers, 1 si à supprimer
 * Résultat :
 *   nombre total de cases marquées à supprimer
 ******************************************************/
int detecterGroupesSimples(const char grille[NB_LIGNES][NB_COLONNES],
                           int marque[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : supprimerGroupes
 * Rôle : remplace par VIDE toutes les cases marquées
 * Paramètres :
 *   grille (IN/OUT) : plateau
 *   marque (IN)     : cases à supprimer (1 = supprimer)
 * Résultat : aucun
 ******************************************************/
void supprimerGroupes(char grille[NB_LIGNES][NB_COLONNES],
                      int marque[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : appliquerGravite
 * Rôle : fait "tomber" les items vers le bas pour
 *        combler les trous dans chaque colonne
 * Paramètres :
 *   grille (IN/OUT) : plateau
 * Résultat : aucun
 ******************************************************/
void appliquerGravite(char grille[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : remplirAvecNouveauxItems
 * Rôle : remplit les cases VIDES avec de nouveaux items
 * Paramètres :
 *   grille (IN/OUT) : plateau
 * Résultat : aucun
 ******************************************************/
void remplirAvecNouveauxItems(char grille[NB_LIGNES][NB_COLONNES]);

/******************************************************
 * Nom : stabiliserGrille
 * Rôle : applique en boucle :
 *        - détection de groupes
 *        - suppression
 *        - gravité + remplissage
 *        jusqu'à ce qu'il n'y ait plus de groupe
 * Paramètres :
 *   grille (IN/OUT) : plateau
 * Résultat : aucun
 ******************************************************/
void stabiliserGrille(char grille[NB_LIGNES][NB_COLONNES]);

#endif // PLANTAMITZ_H

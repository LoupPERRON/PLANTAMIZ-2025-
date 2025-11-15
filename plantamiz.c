#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plantamitz.h"

#ifdef _WIN32
    #include <windows.h>
#endif

/***************** Couleurs console (Windows) *****************/

#ifdef _WIN32
void Color(int couleurDuTexte, int couleurDeFond)
{
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
}
#else
// Version "vide" pour non-Windows (macro si besoin)
void Color(int couleurDuTexte, int couleurDeFond)
{
    (void)couleurDuTexte;
    (void)couleurDeFond;
}
#endif

// renvoie le code couleur pour un item donné
static int couleurPourItem(char item)
{
#ifdef _WIN32
    switch (item)
    {
        case ITEM_SOLEIL:    return 14; // jaune
        case ITEM_FRAISE:    return 12; // rouge
        case ITEM_POMME:     return 10; // vert
        case ITEM_OIGNON:    return 11; // turquoise
        case ITEM_MANDARINE: return 13; // violet/orange-ish
        case VIDE:           return 7;  // gris clair
        default:             return 15; // blanc
    }
#else
    (void)item;
    return 0;
#endif
}

/***************** Fonctions aléatoires & items *****************/

void initialiserAleatoire(void)
{
    srand((unsigned int)time(NULL));
}

char genererItemAleatoire(void)
{
    char items[NB_TYPES_ITEM] = {
        ITEM_SOLEIL,
        ITEM_FRAISE,
        ITEM_POMME,
        ITEM_OIGNON,
        ITEM_MANDARINE
    };

    int index = rand() % NB_TYPES_ITEM;
    return items[index];
}

/***************** Gestion du plateau *****************/

void initialiserGrille(char grille[NB_LIGNES][NB_COLONNES])
{
    int i, j;

    for (i = 0; i < NB_LIGNES; ++i)
    {
        for (j = 0; j < NB_COLONNES; ++j)
        {
            grille[i][j] = genererItemAleatoire();
        }
    }

    // On stabilise : on enlève les groupes initiaux éventuels
    stabiliserGrille(grille);
}

void afficherGrille(const char grille[NB_LIGNES][NB_COLONNES])
{
    int i, j;

    for (i = 0; i < NB_LIGNES; ++i)
    {
        for (j = 0; j < NB_COLONNES; ++j)
        {
            int couleur = couleurPourItem(grille[i][j]);
            Color(couleur, 0);
            printf("%c", grille[i][j]);
        }
        Color(15, 0); // reset
        printf("\n");
    }
}

/***************** Détection des groupes simples *****************/

int detecterGroupesSimples(const char grille[NB_LIGNES][NB_COLONNES],
                           int marque[NB_LIGNES][NB_COLONNES])
{
    int i, j;
    int nbMarques = 0;

    // Réinitialisation de la matrice de marquage
    for (i = 0; i < NB_LIGNES; ++i)
    {
        for (j = 0; j < NB_COLONNES; ++j)
        {
            marque[i][j] = 0;
        }
    }

    // --- Parcours horizontal ---
    for (i = 0; i < NB_LIGNES; ++i)
    {
        j = 0;
        while (j < NB_COLONNES)
        {
            char courant = grille[i][j];
            if (courant == VIDE)
            {
                ++j;
                continue;
            }

            int debut = j;
            int longueur = 1;
            ++j;

            while (j < NB_COLONNES && grille[i][j] == courant)
            {
                ++longueur;
                ++j;
            }

            if (longueur >= 3)
            {
                int k;
                for (k = debut; k < debut + longueur; ++k)
                {
                    if (marque[i][k] == 0)
                    {
                        marque[i][k] = 1;
                        ++nbMarques;
                    }
                }
            }
        }
    }

    // --- Parcours vertical ---
    for (j = 0; j < NB_COLONNES; ++j)
    {
        i = 0;
        while (i < NB_LIGNES)
        {
            char courant = grille[i][j];
            if (courant == VIDE)
            {
                ++i;
                continue;
            }

            int debut = i;
            int longueur = 1;
            ++i;

            while (i < NB_LIGNES && grille[i][j] == courant)
            {
                ++longueur;
                ++i;
            }

            if (longueur >= 3)
            {
                int k;
                for (k = debut; k < debut + longueur; ++k)
                {
                    if (marque[k][j] == 0)
                    {
                        marque[k][j] = 1;
                        ++nbMarques;
                    }
                }
            }
        }
    }

    return nbMarques;
}

/***************** Suppression & gravité *****************/

void supprimerGroupes(char grille[NB_LIGNES][NB_COLONNES],
                      int marque[NB_LIGNES][NB_COLONNES])
{
    int i, j;

    for (i = 0; i < NB_LIGNES; ++i)
    {
        for (j = 0; j < NB_COLONNES; ++j)
        {
            if (marque[i][j] == 1)
            {
                grille[i][j] = VIDE;
            }
        }
    }
}

void appliquerGravite(char grille[NB_LIGNES][NB_COLONNES])
{
    int col, lig;

    for (col = 0; col < NB_COLONNES; ++col)
    {
        int ligneEcriture = NB_LIGNES - 1; // on remplit depuis le bas

        for (lig = NB_LIGNES - 1; lig >= 0; --lig)
        {
            if (grille[lig][col] != VIDE)
            {
                grille[ligneEcriture][col] = grille[lig][col];

                if (ligneEcriture != lig)
                {
                    grille[lig][col] = VIDE;
                }

                --ligneEcriture;
            }
        }

        // Les cases restantes au-dessus sont VIDES
        for (lig = ligneEcriture; lig >= 0; --lig)
        {
            grille[lig][col] = VIDE;
        }
    }
}

void remplirAvecNouveauxItems(char grille[NB_LIGNES][NB_COLONNES])
{
    int lig, col;

    for (lig = 0; lig < NB_LIGNES; ++lig)
    {
        for (col = 0; col < NB_COLONNES; ++col)
        {
            if (grille[lig][col] == VIDE)
            {
                grille[lig][col] = genererItemAleatoire();
            }
        }
    }
}

/***************** Stabilisation *****************/

void stabiliserGrille(char grille[NB_LIGNES][NB_COLONNES])
{
    int marque[NB_LIGNES][NB_COLONNES];
    int nbASupprimer;

    do
    {
        nbASupprimer = detecterGroupesSimples(grille, marque);

        if (nbASupprimer > 0)
        {
            supprimerGroupes(grille, marque);
            appliquerGravite(grille);
            remplirAvecNouveauxItems(grille);
        }

    } while (nbASupprimer > 0);
}

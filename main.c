#include <stdio.h>
#include "plantamitz.h"

int main(void)
{
    char plateau[NB_LIGNES][NB_COLONNES];

    initialiserAleatoire();
    initialiserGrille(plateau);

    printf("Plateau initial stabilise (aucun groupe de 3+ simple) :\n\n");
    afficherGrille(plateau);

    printf("\nAppuyez sur Entree pour quitter...\n");
    getchar();

    return 0;
}

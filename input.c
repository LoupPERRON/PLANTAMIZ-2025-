#include <conio.h> // Pour _getch()
#include <ctype.h>
#include "input.h"

Action recupererAction() {
    int touche = _getch();
    
    // Support des flèches directionnelles (codes étendus)
    if (touche == 0 || touche == 224) {
        touche = _getch();
        switch (touche) {
            case 72: return HAUT;
            case 80: return BAS;
            case 75: return GAUCHE;
            case 77: return DROITE;
        }
    }

    // Support z, q, s, d  et Espace
    switch (tolower(touche)) {
        case 'z': return HAUT;
        case 's': return BAS;
        case 'q': return GAUCHE;
        case 'd': return DROITE;
        case ' ': return SELECTION; // Barre d'espace pour sélectionner 
        case 27: return QUITTER;    // Echap pour quitter
    }

    return RIEN;
}
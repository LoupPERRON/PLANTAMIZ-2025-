#pragma once // Assure que le fichier n'est inclus qu'une seule fois


#include <windows.h> // Inclure la bibliothèque Windows pour les fonctions de console


void gotoligcol(int lig,int col); // Déplace le curseur à la ligne 'lig' et la colonne 'col'
void Color(int couleurDuTexte,int couleurDeFond); // Change la couleur du texte et du fond
void clear_screen(); // Efface l'écran du console
void show_cursor(int visible);  // Show or hide the console cursor
void set_console_mode();        // Configure console input/output mode for Windows
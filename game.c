#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "game.h"
#include "board.h"
#include "console.h"

// structure du jeu permettant de définir les contrats pour chaque niveau
typedef struct {
    int targets[5];
    int maxMoves;
    int tempsSecondes;
} Contrat;

static Contrat contrats[3]; // Contrats pour les 3 niveaux

static void init_contrats() // Initialise les contrats pour chaque niveau
{
    // Level 1: 10 S, 10 F, 10 P, 10 O et 10 M en 30 coups avec 3 minutes max
    memset(&contrats,0,sizeof(contrats));
    contrats[0].targets[0] = 10;   // S
    contrats[0].targets[1] = 10;  // F
    contrats[0].targets[2] = 10;   // P
    contrats[0].targets[3] = 10;  // O
    contrats[0].targets[4] = 10;  // M
    contrats[0].maxMoves = 30;
    contrats[0].tempsSecondes = 180;

    // Level 2: 15 S, 30 P, 20 O et 15 M en 40 mouvements avec 4 minutes max
    contrats[1].targets[0] = 15; // S
    contrats[1].targets[1] = 0; // F
    contrats[1].targets[2] = 30; // P
    contrats[1].targets[3] = 20; // O
    contrats[1].targets[4] = 15;  // M
    contrats[1].maxMoves = 40;
    contrats[1].tempsSecondes = 240;

    // Level 3: 20 S, 20 F, 20 P, 20 O et 20 M en 50 mouvements avec 5 minutes max
    contrats[2].targets[0] = 20; // S
    contrats[2].targets[1] = 20; // F
    contrats[2].targets[2] = 20; // P
    contrats[2].targets[3] = 20; // O
    contrats[2].targets[4] = 20; // M
    contrats[2].maxMoves = 50; // max moves
    contrats[2].tempsSecondes = 300; // temps en secondes
}

static void sauvegarder(const char *nom,int nouveau_niveau) // Sauvegarde la progression du joueur dans un fichier
{
    FILE *f = fopen("savegame.txt","w"); // Ouvre le fichier en écriture
    if(!f) return; // Si l'ouverture échoue, retourne
    fprintf(f, "%s %d\n", nom, nouveau_niveau); // Écrit le nom du joueur et le niveau suivant
    fclose(f); // Ferme le fichier
}

static int charger(const char *nom) // Charge la progression du joueur depuis un fichier
{
    FILE *f = fopen("savegame.txt","r"); // Ouvre le fichier en lecture
    if(!f) return 0; // Si l'ouverture échoue, retourne 0
    char saved[128]; int lvl=0; // Variables pour stocker le nom sauvegardé et le niveau
    while(fscanf(f, "%127s %d", saved, &lvl)==2){ // Lit chaque ligne du fichier
        if(strcmp(saved,nom)==0){ fclose(f); return lvl; } // Si le nom correspond, retourne le niveau
    }
    fclose(f); // Ferme le fichier
    return 0; // Si le nom n'est pas trouvé, retourne 0
}

static void print_status(int level,int moves_left,int vies,int points, int progress[5], Contrat *c){ // Affiche le statut actuel du jeu
    gotoligcol(0,0); // Déplace le curseur en haut à gauche
    printf("Niveau %d   Coups restants: %d   Vies: %d   Points: %d\n", level+1,moves_left,vies,points); // Affiche le niveau, les mouvements restants, les vies et les points
    printf("Contrat: S:%d F:%d P:%d O:%d M:%d\n", // Affiche les objectifs du contrat
           c->targets[0], c->targets[1], c->targets[2], c->targets[3], c->targets[4]); // Objectifs
    printf("Progres: S:%d F:%d P:%d O:%d M:%d\n", // Affiche la progression actuelle
           progress[0],progress[1],progress[2],progress[3],progress[4]); // Progression
}

void game_run(int reprendre) // Démarre le jeu, reprendre: 0=nouvelle partie, 1=reprise depuis la sauvegarde
{
    init_contrats(); // Initialise les contrats pour chaque niveau

    char joueur[64]; // Nom du joueur
    printf("Entrez votre nom de joueur: "); //  Demande le nom du joueur
    if(!fgets(joueur,sizeof(joueur),stdin)) strcpy(joueur,"joueur\n"); // lire le nom du joueur
    // supprimer le retour à la ligne
    joueur[strcspn(joueur,"\r\n")] = '\0'; 
    int resume_level = charger(joueur); // Charger la progression du joueur
    if(!reprendre) resume_level = 0; // Si la reprise n'est pas autorisée, commencer une nouvelle partie

    
 // variables de jeu
    Tableau b;
    int cursor_r=0,cursor_c=0;
    int selected_r=-1, selected_c=-1;
    int vies=5;
    int level = resume_level;
    if(level<0) level=0;
    if(level>2) level=2;
    int points=0;

    for(; level<3; level++){ // boucle à travers les niveaux
        init_tableau(&b);
        int moves_left = contrats[level].maxMoves;
        int progress[5] = {0,0,0,0,0};
        time_t start = time(NULL);
        (void)0;

        while(1){ // boucle principale du niveau
            effacer_ecran();
            print_status(level,moves_left,vies,points,progress,&contrats[level]);
            Tableau_print(&b,cursor_r,cursor_c,selected_r,selected_c);
            printf("Controls: z q s d pour bouger, ESPACE pour choisir/echanger, p pour quitter\n");
            {
                int restant = contrats[level].tempsSecondes - (int)(time(NULL)-start);
                if(restant < 0) restant = 0;
                printf("Temps restants: %d secondes\n", restant );
            }

            // boucle d'entrée non bloquante : attendre l'appui d'une touche et le minuteur visible
            while(!kbhit()){
                //  mettre à jour l'affichage du temps de temps en temps
                Sleep(80);
                int ecoule = (int)(time(NULL)-start);
                if(ecoule >= contrats[level].tempsSecondes){
                    // fin du temps
                    moves_left = 0;
                    break;
                }
            }
            int ch = _getch(); // lire le caractère saisi
            if(ch== 'p'){
                printf("Quitter et sauvegarder progression.\n");
                sauvegarder(joueur, level);
                return;
            }
            if(ch== 'z' && cursor_r>0) cursor_r--; // déplacer le curseur vers le haut
            else if(ch=='s' && cursor_r<LIGNES-1) cursor_r++; // déplacer le curseur vers le bas
            else if(ch=='q' && cursor_c>0) cursor_c--; // déplacer le curseur vers la gauche
            else if(ch=='d' && cursor_c<COLONNES-1) cursor_c++; // déplacer le curseur vers la droite
            else if(ch==' '){ // sélectionner ou échanger
                if(selected_r==-1){ selected_r=cursor_r; selected_c=cursor_c; }
                else{
                    int dr = abs(selected_r - cursor_r);
                    int dc = abs(selected_c - cursor_c);
                    if((dr==1 && dc==0) || (dr==0 && dc==1)){
                        // sauvegarde des comptes
                        int before[5]; for(int i=0;i<5;i++) before[i]=Tableau_count_char(&b, "SFPOM"[i]);
                        Tableau_swap(&b, selected_r,selected_c,cursor_r,cursor_c);
                        int gained=0;
                        if(!Tableau_find_and_remove_matches(&b,&gained)){
                            Tableau_swap(&b, selected_r,selected_c,cursor_r,cursor_c);
                        } else {
                            // calculer les quantités supprimées
                            int after[5]; for(int i=0;i<5;i++) after[i]=Tableau_count_char(&b, "SFPOM"[i]);
                            for(int i=0;i<5;i++){
                                int removed = before[i]-after[i];
                                if(removed>0){ progress[i]+=removed; }
                            }
                            points += gained;
                            moves_left--;
                        }
                    }
                    selected_r=-1; selected_c=-1;
                }
            }

            // check completion
            int ok=1;
            for(int i=0;i<5;i++) if(progress[i] < contrats[level].targets[i]) ok=0;
            if(ok){
                printf("Niveau %d terminé ! Appuyez sur une touche pour continuer.\n", level+1);
                _getch();
                //  vérification de la réussite du contrat
                sauvegarder(joueur, level+1);
                break; //allez vers le prochain niveau
            }

            if(moves_left<=0){
                vies--;
                if(vies<=0){
                    printf("Fin de la partie. Vous avez marqué %d points.\n", points);
                    sauvegarder(joueur, 0);
                    return;
                } else {
                    printf("Niveau impossible. Vies restantes : %d. Appuyez sur une touche pour réessayer.\n", vies);
                    _getch();
                    init_tableau(&b);
                    moves_left = contrats[level].maxMoves;
                    for(int i=0;i<5;i++) progress[i]=0;
                    // réinitialiser le minuteur du niveau pour éviter qu'il n'expire immédiatement
                    start = time(NULL);
                }
            }
        }
    }

    printf("Bravo! Vous avez terminé les 3 tableaux. Points totaux: %d\n", points);
    sauvegarder(joueur, 3);
}
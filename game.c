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
    // niveau 1: 10 S, 10 F, 10 P, 10 O et 10 M en 30 coups avec 3 minutes max
    memset(&contrats,0,sizeof(contrats));
    contrats[0].targets[0] = 10;   // S
    contrats[0].targets[1] = 10;  // F
    contrats[0].targets[2] = 10;   // P
    contrats[0].targets[3] = 10;  // O
    contrats[0].targets[4] = 10;  // M
    contrats[0].maxMoves = 17;
    contrats[0].tempsSecondes = 80;

    // niveau 2: 15 S, 30 P, 20 O et 15 M en 40 mouvements avec 4 minutes max
    contrats[1].targets[0] = 15; // S
    contrats[1].targets[1] = 0; // F
    contrats[1].targets[2] = 30; // P
    contrats[1].targets[3] = 20; // O
    contrats[1].targets[4] = 15;  // M
    contrats[1].maxMoves = 25;
    contrats[1].tempsSecondes = 160;

    // niveau 3: 20 S, 20 F, 20 P, 20 O et 20 M en 50 mouvements avec 5 minutes max
    contrats[2].targets[0] = 20; // S
    contrats[2].targets[1] = 20; // F
    contrats[2].targets[2] = 20; // P
    contrats[2].targets[3] = 20; // O
    contrats[2].targets[4] = 20; // M
    contrats[2].maxMoves = 30; // max moves
    contrats[2].tempsSecondes = 240; // temps en secondes
}

static void sauvegarder(const char *nom,int nouveau_niveau) // Sauvegarde la progresion du joueur dans un fichier
{
    FILE *f = fopen("savegame.txt","w"); // Ouvre le fichier en écriture
    if(!f) return; // Si l'ouverture échoue, retourne
    fprintf(f, "%s %d\n", nom, nouveau_niveau); // Écrit le nom du joueur et le niveau suivant
    fclose(f); // Ferme le fichier
}

static int charger(const char *nom) // Charge la progresion du joueur depuis un fichier
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

static void afficher_statut(int niveau,int coups_restants,int vies,int points, int progres[5], Contrat *c){ // Affiche le statut actuel du jeu
    gotoligcol(0,0); // Déplace le curseur en haut à gauche
    printf("Niveau %d   Coups restants: %d   Vies: %d   Points: %d\n", niveau+1,coups_restants,vies,points); // Affiche le niveau, les mouvements restants, les vies et les points
    printf("Contrat: S:%d F:%d P:%d O:%d M:%d\n", // Affiche les objectifs du contrat
           c->targets[0], c->targets[1], c->targets[2], c->targets[3], c->targets[4]); // Objectifs
    printf("Progres: S:%d F:%d P:%d O:%d M:%d\n", // Affiche la progresion actuelle
           progres[0],progres[1],progres[2],progres[3],progres[4]); // progresion
}

void game_run(int reprendre) // Démarre le jeu, reprendre: 0=nouvelle partie, 1=reprise depuis la sauvegarde
{
    init_contrats(); // Initialise les contrats pour chaque niveau

    char joueur[64]; // Nom du joueur
    printf("Entrez votre nom de joueur: "); //  Demande le nom du joueur
    if(!fgets(joueur,sizeof(joueur),stdin)) strcpy(joueur,"joueur\n"); // lire le nom du joueur
    // supprimer le retour à la ligne
    joueur[strcspn(joueur,"\r\n")] = '\0'; 
    int reprendre_niveau = charger(joueur); // Charger la progresion du joueur
    if(!reprendre) reprendre_niveau = 0; // Si la reprise n'est pas autorisée, commencer une nouvelle partie

    
 // variables de jeu
    Tableau b;
    int curseur_l=0,curseur_c=0;
    int choisi_l=-1, choisi_c=-1;
    int vies=5;
    int niveau = reprendre_niveau;
    if(niveau<0) niveau=0;
    if(niveau>2) niveau=2;
    int points=0;

    for(; niveau<3; niveau++){ // boucle à travers les niveaux
        init_tableau(&b);
        int coups_restants = contrats[niveau].maxMoves;
        int progres[5] = {0,0,0,0,0};
        time_t debut = time(NULL);
        (void)0;

        while(1){ // boucle principale du niveau
            effacer_ecran();
            afficher_statut(niveau,coups_restants,vies,points,progres,&contrats[niveau]);
            Tableau_print(&b,curseur_l,curseur_c,choisi_l,choisi_c);
            printf("Controls: z q s d pour bouger, ESPACE pour choisir/echanger, p pour quitter\n");
            {
                int restant = contrats[niveau].tempsSecondes - (int)(time(NULL)-debut);
                if(restant < 0) restant = 0;
                printf("Temps restants: %d secondes\n", restant );
            }

            // boucle d'entrée non bloquante : attendre l'appui d'une touche
            // Actualise le timer visuel toutes les ~80ms en réécrivant la ligne "Temps restants"
            {
                int ligne_temps = 3 + LIGNES + 2; // base_row(3) + LIGNES + 2 -> ligne où "Temps restants" est affiché
                while(!kbhit()){
                    Sleep(80);
                    int ecoule = (int)(time(NULL)-debut);
                    int restant = contrats[niveau].tempsSecondes - ecoule;
                    if(restant < 0) restant = 0;
                    gotoligcol(ligne_temps,0);
                    // réécrire la ligne du temps (sans saut de ligne) et forcer flush
                    printf("Temps restants: %d secondes   ", restant);
                    fflush(stdout);
                    if(ecoule >= contrats[niveau].tempsSecondes){
                        // fin du temps
                        coups_restants = 0;
                        break;
                    }
                }
            }
            int ch = _getch(); // lire le caractère saisi
            if(ch== 'p'){
                printf("Quitter et sauvegarder progresion.\n");
                sauvegarder(joueur, niveau);
                return;
            }
            if(ch== 'z' && curseur_l>0) curseur_l--; // déplacer le curseur vers le haut
            else if(ch=='s' && curseur_l<LIGNES-1) curseur_l++; // déplacer le curseur vers le bas
            else if(ch=='q' && curseur_c>0) curseur_c--; // déplacer le curseur vers la gauche
            else if(ch=='d' && curseur_c<COLONNES-1) curseur_c++; // déplacer le curseur vers la droite
            else if(ch==' '){ // sélectionner ou échanger
                if(choisi_l==-1){
                    choisi_l = curseur_l; choisi_c = curseur_c;
                } else {
                    int dr = abs(choisi_l - curseur_l);
                    int dc = abs(choisi_c - curseur_c);
                    if((dr==1 && dc==0) || (dr==0 && dc==1)){
                        Tableau_swap(&b, choisi_l,choisi_c,curseur_l,curseur_c);
                        int gained=0;
                        int removed[5] = {0,0,0,0,0};
                        // appeler la version animée de la suppression (delai en ms)
                        if(!Tableau_trouver_et_supprimer_les_correspondances(&b,&gained, removed, 600, curseur_l, curseur_c, choisi_l, choisi_c, niveau, coups_restants, vies, points, progres, contrats[niveau].targets)){
                            Tableau_swap(&b, choisi_l,choisi_c,curseur_l,curseur_c);
                        } else {
                            for(int i=0;i<5;i++) if(removed[i]>0) progres[i]+=removed[i];
                            points += gained;
                            coups_restants--;
                            // mettre à jour immédiatement l'affichage du statut et du plateau
                            effacer_ecran();
                            afficher_statut(niveau,coups_restants,vies,points,progres,&contrats[niveau]);
                            Tableau_print(&b,curseur_l,curseur_c,choisi_l,choisi_c);
                            fflush(stdout);
                        }
                    }
                    choisi_l=-1; choisi_c=-1;
                }
            }
            
            // check completion
            int ok=1;
            for(int i=0;i<5;i++) if(progres[i] < contrats[niveau].targets[i]) ok=0;
            if(ok){
                printf("Niveau %d termine ! Appuyez sur une touche pour continuer.\n", niveau+1); // niveau terminé
                _getch();
                //  vérification de la réussite du contrat
                sauvegarder(joueur, niveau+1);
                break; //allez vers le prochain niveau
            }

            if(coups_restants<=0){ // vérifier l'échec du niveau
                vies--; // perdre une vie
                if(vies<=0){ // vérifier la fin du jeu
                    printf("Fin de la partie. Vous avez marque %d points.\n", points);
                    sauvegarder(joueur, 0);
                    return;
                } else {
                    printf("Niveau impossible. Vies restantes : %d. Appuyez sur une touche pour reessayer.\n", vies);
                    _getch();
                    init_tableau(&b);
                    coups_restants = contrats[niveau].maxMoves;
                    for(int i=0;i<5;i++) progres[i]=0;
                    // réinitialiser le minuteur du niveau pour éviter qu'il n'expire immédiatement
                    debut = time(NULL); // reset timer
                }
            }
        }
    }

    printf("Bravo! Vous avez termine les 3 tableaux. Points totaux: %d\n", points);
    sauvegarder(joueur, 3);
}
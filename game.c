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
    int timeSec;
} Contract;

static Contract contracts[3];

static void init_contracts()
{
    // Level 1: 10 S, 10 F, 10 P, 10 O et 10 M en 30 coups avec 3 minutes max
    memset(&contracts,0,sizeof(contracts));
    contracts[0].targets[0] = 10;   // S
    contracts[0].targets[1] = 10;  // F
    contracts[0].targets[2] = 10;   // P
    contracts[0].targets[3] = 10;  // O
    contracts[0].targets[4] = 10;  // M
    contracts[0].maxMoves = 30;
    contracts[0].timeSec = 180;

    // Level 2: 15 S, 30 P, 20 O et 15 M en 40 mouvements avec 4 minutes max
    contracts[1].targets[0] = 15; // S
    contracts[1].targets[1] = 0; // F
    contracts[1].targets[2] = 30; // P
    contracts[1].targets[3] = 20; // O
    contracts[1].targets[4] = 15;  // M
    contracts[1].maxMoves = 40;
    contracts[1].timeSec = 240;

    // Level 3: 20 S, 20 F, 20 P, 20 O et 20 M en 50 mouvements avec 5 minutes max
    contracts[2].targets[0] = 20; // S
    contracts[2].targets[1] = 20; // F
    contracts[2].targets[2] = 20; // P
    contracts[2].targets[3] = 20; // O
    contracts[2].targets[4] = 20; // M
    contracts[2].maxMoves = 50;
    contracts[2].timeSec = 300;
}

static void save_progress(const char *name,int next_level)
{
    FILE *f = fopen("savegame.txt","w");
    if(!f) return;
    fprintf(f, "%s %d\n", name, next_level);
    fclose(f);
}

static int load_progress(const char *name)
{
    FILE *f = fopen("savegame.txt","r");
    if(!f) return 0;
    char saved[128]; int lvl=0;
    while(fscanf(f, "%127s %d", saved, &lvl)==2){
        if(strcmp(saved,name)==0){ fclose(f); return lvl; }
    }
    fclose(f);
    return 0;
}

static void print_status(int level,int moves_left,int lives,int points, int progress[5], Contract *c){
    gotoligcol(0,0);
    printf("Level %d   Moves left: %d   Lives: %d   Points: %d\n", level+1,moves_left,lives,points);
    printf("Contract: S:%d F:%d P:%d O:%d M:%d\n",
           c->targets[0], c->targets[1], c->targets[2], c->targets[3], c->targets[4]);
    printf("Progress: S:%d F:%d P:%d O:%d M:%d\n",
           progress[0],progress[1],progress[2],progress[3],progress[4]);
}

void game_run(int allow_resume)
{
    init_contracts();

    char player[64];
    printf("Entrez votre nom de joueur: ");
    if(!fgets(player,sizeof(player),stdin)) strcpy(player,"player\n");
    // supprimer le retour à la ligne
    player[strcspn(player,"\r\n")] = '\0';
    int resume_level = load_progress(player);
    if(!allow_resume) resume_level = 0;

    

    Board b;
    int cursor_r=0,cursor_c=0;
    int selected_r=-1, selected_c=-1;
    int lives=5;
    int level = resume_level;
    if(level<0) level=0;
    if(level>2) level=2;
    int points=0;

    for(; level<3; level++){
        board_init(&b);
        int moves_left = contracts[level].maxMoves;
        int progress[5] = {0,0,0,0,0};
        time_t start = time(NULL);
        (void)0;

        while(1){
            clear_screen();
            print_status(level,moves_left,lives,points,progress,&contracts[level]);
            board_print(&b,cursor_r,cursor_c,selected_r,selected_c);
            printf("Controls: z q s d to move, SPACE to select/swap, p to quit\n");
            {
                int remaining = contracts[level].timeSec - (int)(time(NULL)-start);
                if(remaining < 0) remaining = 0;
                printf("Time left: %d seconds\n", remaining );
            }

            // boucle d'entrée non bloquante : attendre l'appui d'une touche et le minuteur visible
            while(!kbhit()){
                //  mettre à jour l'affichage du temps de temps en temps
                Sleep(80);
                int elapsed = (int)(time(NULL)-start);
                if(elapsed >= contracts[level].timeSec){
                    // fin du temps
                    moves_left = 0;
                    break;
                }
            }
            int ch = _getch();
            if(ch== 'p'){
                printf("Quitter et sauvegarder progression.\n");
                save_progress(player, level);
                return;
            }
            if(ch== 'z' && cursor_r>0) cursor_r--;
            else if(ch=='s' && cursor_r<ROWS-1) cursor_r++;
            else if(ch=='q' && cursor_c>0) cursor_c--;
            else if(ch=='d' && cursor_c<COLS-1) cursor_c++;
            else if(ch==' '){
                if(selected_r==-1){ selected_r=cursor_r; selected_c=cursor_c; }
                else{
                    int dr = abs(selected_r - cursor_r);
                    int dc = abs(selected_c - cursor_c);
                    if((dr==1 && dc==0) || (dr==0 && dc==1)){
                        // sauvegarde des comptes
                        int before[5]; for(int i=0;i<5;i++) before[i]=board_count_char(&b, "SFPOM"[i]);
                        board_swap(&b, selected_r,selected_c,cursor_r,cursor_c);
                        int gained=0;
                        if(!board_find_and_remove_matches(&b,&gained)){
                            board_swap(&b, selected_r,selected_c,cursor_r,cursor_c);
                        } else {
                            // calculer les quantités supprimées
                            int after[5]; for(int i=0;i<5;i++) after[i]=board_count_char(&b, "SFPOM"[i]);
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
            for(int i=0;i<5;i++) if(progress[i] < contracts[level].targets[i]) ok=0;
            if(ok){
                printf("Level %d completed! Press any key to continue.\n", level+1);
                _getch();
                //  vérification de la réussite du contrat
                save_progress(player, level+1);
                break; //allez vers le prochain niveau
            }

            if(moves_left<=0){
                lives--;
                if(lives<=0){
                    printf("Game over. You scored %d points.\n", points);
                    save_progress(player, 0);
                    return;
                } else {
                    printf("Level failed. Lives left: %d. Press any key to retry level.\n", lives);
                    _getch();
                    board_init(&b);
                    moves_left = contracts[level].maxMoves;
                    for(int i=0;i<5;i++) progress[i]=0;
                    // réinitialiser le minuteur du niveau pour éviter qu'il n'expire immédiatement
                    start = time(NULL);
                }
            }
        }
    }

    printf("Bravo! Vous avez terminé les 3 tableaux. Points totaux: %d\n", points);
    save_progress(player, 3);
}
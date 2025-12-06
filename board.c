#include "board.h" // Inclure le fichier d'en-tête pour la structure Board et les prototypes de fonctions
#include <stdlib.h> // Pour les fonctions rand() et srand()
#include <time.h> // Pour la fonction time()
#include <string.h> // Pour la fonction memset()
#include <stdio.h> // Pour les fonctions d'entrée/sortie standard
#include <ctype.h> // Pour la fonction tolower()
#include "console.h" // Inclure le fichier d'en-tête pour les fonctions de manipulation de la console

static const char ITEMS[] = {'S','F','P','O','M'}; // S=Soleil, F=Fraise, P=Plant, O=Ocean, M=Mountain
static const int NITEMS = 5; // Nombre d'items différents

static char element_alea()    // Génère un caractère aléatoire parmi les ITEMS
{
    return ITEMS[rand() % NITEMS]; // Retourne un caractère aléatoire parmi les ITEMS
}

void board_init(Board *b) // Initialise le plateau avec des caractères aléatoires sans correspondances initiales
{
    srand((unsigned)time(NULL)); // Initialiser le générateur de nombres aléatoires avec le temps actuel
    // Génère des plateaux jusqu'à MAX_ATTEMPTS afin d'obtenir un plateau sans correspondances
    // évite une boucle  qui rééchantillonne des cellules aléatoires
    const int MAX_ATTEMPTS = 1000;
    int attempts = 0; // Compteur de tentatives
    do {
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                b->cells[r][c] = element_alea();
            }
        }
        attempts++;
        if(attempts >= MAX_ATTEMPTS) break;
    } while(!board_no_initial_matches(b));

    if(attempts >= MAX_ATTEMPTS){
        //Repli : si aucun plateau n'a été trouvé,casser les triples en rééchantillonnant les cellules .
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                // triple horizontal commençant ici
                if(c <= COLS-3){
                    if(b->cells[r][c] != '\0' && b->cells[r][c] == b->cells[r][c+1] && b->cells[r][c] == b->cells[r][c+2]){
                        b->cells[r][c] = element_alea();
                    }
                }
                // triple vertical commençant ici
                if(r <= ROWS-3){
                    if(b->cells[r][c] != '\0' && b->cells[r][c] == b->cells[r+1][c] && b->cells[r][c] == b->cells[r+2][c]){
                        b->cells[r][c] = element_alea();
                    }
                }
            }
        }
    }
}

static int color_for_char(char ch)
{
    switch(ch){
        case 'S': return 14; // jaune
        case 'F': return 12; // rouge
        case 'P': return 10; // vert
        case 'O': return 3;  // turquoise
        case 'M': return 6;  // marron
        default: return 15;  // blanc
    }
}

void board_print(Board *b, int cursor_r, int cursor_c, int selected_r, int selected_c)
{
    // Si le tampon de la console est trop petit pour positionner les caractères en sécurité, revenir à un affichage simple
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GetConsoleScreenBufferInfo(h, &csbi)){
        //impossible d'obtenir les informations de la console -> revenir à l'affichage ASCII
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                char ch = b->cells[r][c]; if(ch=='\0') ch=' ';
                if(r==selected_r && c==selected_c) putchar(tolower(ch));
                else if(r==cursor_r && c==cursor_c) { putchar('['); putchar(ch); putchar(']'); }
                else putchar(ch);
            }
            putchar('\n');
        }
        fflush(stdout);
        return;
    } else {
        int width = csbi.dwSize.X;
        int height = csbi.dwSize.Y;
        if(width < COLS*2 || height < ROWS+6){
            //repli : retour à un affichage ASCII simple
            for(int r=0;r<ROWS;r++){
                for(int c=0;c<COLS;c++){
                    char ch = b->cells[r][c]; if(ch=='\0') ch=' ';
                    if(r==selected_r && c==selected_c) putchar(tolower(ch));
                    else if(r==cursor_r && c==cursor_c) { putchar('['); putchar(ch); putchar(']'); }
                    else putchar(ch);
                }
                putchar('\n');
            }
            fflush(stdout);
            return;
        }
    }

    //marge supérieure
    int base_row = 3;
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            int scr_r = base_row + r;
            int scr_c = c*2; // espace entre les colonnes
            gotoligcol(scr_r, scr_c);
            char ch = b->cells[r][c];
            if(ch == '\0') ch = ' ';
            int col = color_for_char(ch);
            if(r==selected_r && c==selected_c){
                //sélectionné : caractère en arrière-plan lumineux
                Color(col, 4);
                putchar(tolower(ch));
                Color(15,0);
            } else if(r==cursor_r && c==cursor_c){
                //curseur : arrière-plan inversé
                Color(0, 14);
                putchar(ch);
                Color(15,0);
            } else {
                Color(col,0);
                putchar(ch);
                Color(15,0);
            }
        }
    }
    // déplacer curseur après affichage du plateau
    gotoligcol(base_row+ROWS+1,0);
    fflush(stdout);
}

void board_apply_gravity(Board *b) // Applique la gravité pour faire tomber les caractères
{
    for(int c=0;c<COLS;c++){ // pour chaque colonne
        int write = ROWS-1;
        for(int r=ROWS-1;r>=0;r--){
            if(b->cells[r][c] != '\0'){
                b->cells[write][c] = b->cells[r][c];
                write--;
            }
        }
        for(int r=write;r>=0;r--) b->cells[r][c] = element_alea();
    }
}

int board_count_char(Board *b,char ch) // Compte le nombre d'occurrences d'un caractère donné sur le plateau
{
    int cnt=0; for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(b->cells[r][c]==ch) cnt++; return cnt;
}

void board_swap(Board *b, int r1,int c1,int r2,int c2) // Échange deux cellules du plateau
{
    char tmp = b->cells[r1][c1];
    b->cells[r1][c1] = b->cells[r2][c2];
    b->cells[r2][c2] = tmp;
}

//Trouve les séquences horizontales et verticales >=3,rectangles et formes en H
//Marque les cellules à supprimer et met à jour la grille , renvoie true si une suppression a eu lieu et incrémente les points
bool board_find_and_remove_matches(Board *b, int *points)
{
    bool any_global = false;
    int points_accum = 0;

    //Répéter jusqu'à stabilisation
    while(1){
        bool remove[ROWS][COLS];
        memset(remove,0,sizeof(remove));
        bool any=false;

        // horizontal
        for(int r=0;r<ROWS;r++){
            int c=0;
            while(c<COLS){
                char base = b->cells[r][c];
                int j=c+1;
                while(j<COLS && base!='\0' && b->cells[r][j]==base) j++;
                int len = j-c;
                if(base!='\0' && len>=3){
                    any=true;
                    for(int k=c;k<j;k++) remove[r][k]=true;
                    if(len==4) points_accum += 4;
                    else if(len==6){
                        // remove all identical
                        char ch = base;
                        int total = board_count_char(b,ch);
                        points_accum += total;
                        for(int rr=0;rr<ROWS;rr++) for(int cc=0;cc<COLS;cc++) if(b->cells[rr][cc]==ch) remove[rr][cc]=true;
                    } else {
                        points_accum += len;
                    }
                }
                c=j;
            }
        }

        // vertical
        for(int c=0;c<COLS;c++){
            int r=0;
            while(r<ROWS){
                char base = b->cells[r][c];
                int i=r+1;
                while(i<ROWS && base!='\0' && b->cells[i][c]==base) i++;
                int len = i-r;
                if(base!='\0' && len>=3){
                    any=true;
                    for(int k=r;k<i;k++) remove[k][c]=true;
                    if(len==4) points_accum += 4;
                    else if(len==6){
                        char ch = base;
                        int total = board_count_char(b,ch);
                        points_accum += total;
                        for(int rr=0;rr<ROWS;rr++) for(int cc=0;cc<COLS;cc++) if(b->cells[rr][cc]==ch) remove[rr][cc]=true;
                    } else {
                        points_accum += len;
                    }
                }
                r=i;
            }
        }

        //détection de rectangles (brute-force) d'au moins 2x2
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                char ch = b->cells[r][c];
                if(!ch) continue;
                for(int h=2;r+h-1<ROWS;h++){
                    for(int w=2;c+w-1<COLS;w++){
                        bool ok=true;
                        for(int rr=r;rr<r+h && ok;rr++) for(int cc=c;cc<c+w;cc++) if(b->cells[rr][cc]!=ch) {ok=false; break;}
                        if(ok){
                            any=true;
                            for(int rr=r;rr<r+h;rr++) for(int cc=c;cc<c+w;cc++) remove[rr][cc]=true;
                            points_accum += 2*(h*w);
                        }
                    }
                }
            }
        }

        //Détection en H : deux barres verticales de 3 reliées par une barre horizontale centrale (motif 3x3)
        for(int r=0;r+2<ROWS;r++){
            for(int c=0;c+2<COLS;c++){
                char ch = b->cells[r][c];
                if(ch && b->cells[r+1][c]==ch && b->cells[r+2][c]==ch && b->cells[r][c+2]==ch && b->cells[r+1][c+2]==ch && b->cells[r+2][c+2]==ch && b->cells[r+1][c+1]==ch){
                    any=true;
                    for(int rr=r;rr<r+3;rr++) for(int cc=c;cc<c+3;cc++) remove[rr][cc]=true;
                    points_accum += 2*9; //2 * X où X = nombre d'items (ici 9)
                }
            }
        }

        if(!any) break;

        any_global = true;
        //appliquer les suppressions
        for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(remove[r][c]) b->cells[r][c]='\0';
        board_apply_gravity(b);
        //continuer  boucle pour détecter cascades
    }

    if(points) *points += points_accum;
    return any_global;
}

bool board_no_initial_matches(Board *b)
{
    //vérifier séquences horizontales/verticales >= 3
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS-2;c++){
            char ch = b->cells[r][c]; if(ch==0) continue;
            if(b->cells[r][c+1]==ch && b->cells[r][c+2]==ch) return false;
        }
    }
    for(int c=0;c<COLS;c++){
        for(int r=0;r<ROWS-2;r++){
            char ch = b->cells[r][c]; if(ch==0) continue;
            if(b->cells[r+1][c]==ch && b->cells[r+2][c]==ch) return false;
        }
    }
    return true;
}

#include "board.h" // Inclure le fichier d'en-tête pour la structure Tableau et les prototypes de fonctions
#include <stdlib.h> // Pour les fonctions rand() et srand()
#include <time.h> // Pour la fonction time()
#include <string.h> // Pour la fonction memset()
#include <stdio.h> // Pour les fonctions d'entrée/sortie standard
#include <ctype.h> // Pour la fonction tolower()
#include "console.h" // Inclure le fichier d'en-tête pour les fonctions de manipulation de la console

static const char ELEMENTS[] = {'S','F','P','O','M'}; // S=Soleil, F=Fraise, P=Pomme, O=Ognion, M=Mandarine
static const int NELEMENTS = 5; // Nombre d'ELEMENTS différents

static char element_alea()    // Génère un caractère aléatoire parmi les ELEMENTS
{
    return ELEMENTS[rand() % NELEMENTS]; // Retourne un caractère aléatoire parmi les ELEMENTS
}

void init_tableau(Tableau *b) // Initialise le plateau avec des caractères aléatoires sans correspondances initiales
{
    srand((unsigned)time(NULL)); // Initialiser le générateur de nombres aléatoires avec le temps actuel
    // Génère des plateaux jusqu'à TENTATIVES_MAX afin d'obtenir un plateau sans correspondances
    // évite une boucle  qui rééchantillonne des cellules aléatoires
    const int TENTATIVES_MAX = 1000;
    int tentatives = 0; // Compteur de tentatives
    do {
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                b->cellules[r][c] = element_alea();
            }
        }
        tentatives++;
        if(tentatives >= TENTATIVES_MAX) break;
    } while(!Tableau_pas_de_correspondances_initiales(b));

    if(tentatives >= TENTATIVES_MAX){
        //Repli : si aucun plateau n'a été trouvé,casser les triples en rééchantillonnant les cellules .
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                // triple horizontal commençant ici
                if(c <= COLONNES-3){
                    if(b->cellules[r][c] != '\0' && b->cellules[r][c] == b->cellules[r][c+1] && b->cellules[r][c] == b->cellules[r][c+2]){
                        b->cellules[r][c] = element_alea();
                    }
                }
                // triple vertical commençant ici
                if(r <= LIGNES-3){
                    if(b->cellules[r][c] != '\0' && b->cellules[r][c] == b->cellules[r+1][c] && b->cellules[r][c] == b->cellules[r+2][c]){
                        b->cellules[r][c] = element_alea();
                    }
                }
            }
        }
    }
}

static int couleur_car(char ch) // Retourne la couleur associée à un caractère
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

void Tableau_print(Tableau *b, int cursor_r, int cursor_c, int selected_r, int selected_c)
{
    // Si le tampon de la console est trop petit pour positionner les caractères en sécurité, revenir à un affichage simple
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GetConsoleScreenBufferInfo(h, &csbi)){
        //impossible d'obtenir les informations de la console -> revenir à l'affichage ASCII
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                char ch = b->cellules[r][c]; if(ch=='\0') ch=' ';
                if(r==selected_r && c==selected_c) putchar(tolower(ch));
                else if(r==cursor_r && c==cursor_c) { putchar('['); putchar(ch); putchar(']'); }
                else putchar(ch);
            }
            putchar('\n');
        }
        fflush(stdout);
        return;
    } else {
        int largeur = csbi.dwSize.X;
        int hauteur = csbi.dwSize.Y;
        if(largeur < COLONNES*2 || hauteur < LIGNES+6){
            //repli : retour à un affichage ASCII simple
            for(int r=0;r<LIGNES;r++){
                for(int c=0;c<COLONNES;c++){
                    char ch = b->cellules[r][c]; if(ch=='\0') ch=' ';
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
    for(int r=0;r<LIGNES;r++){ // pour chaque ligne
        for(int c=0;c<COLONNES;c++){
            int scr_r = base_row + r;
            int scr_c = c*2; // espace entre les colonnes
            gotoligcol(scr_r, scr_c);
            char ch = b->cellules[r][c];
            if(ch == '\0') ch = ' '; // cellule vide
            int col = couleur_car(ch);
            if(r==selected_r && c==selected_c){ 
                //sélectionné : caractère en arrière-plan lumineux
                Couleur(col, 4);
                putchar(tolower(ch));
                Couleur(15,0);
            } else if(r==cursor_r && c==cursor_c){
                //curseur : arrière-plan inversé
                Couleur(0, 14);
                putchar(ch);
                Couleur(15,0);
            } else {
                Couleur(col,0);
                putchar(ch);
                Couleur(15,0);
            }
        }
    }
    // déplacer curseur après affichage du plateau
    gotoligcol(base_row+LIGNES+1,0);
    fflush(stdout);
}

void Tableau_appliquer_gravité(Tableau *b) // Applique la gravité pour faire tomber les caractères
{
    for(int c=0;c<COLONNES;c++){ // pour chaque colonne
        int ecrire = LIGNES-1;
        for(int r=LIGNES-1;r>=0;r--){
            if(b->cellules[r][c] != '\0'){
                b->cellules[ecrire][c] = b->cellules[r][c];
                ecrire--;
            }
        }
        for(int r=ecrire;r>=0;r--) b->cellules[r][c] = element_alea();
    }
}

int Tableau_count_char(Tableau *b,char ch) // Compte le nombre d'occurrences d'un caractère donné sur le plateau
{
    int cnt=0; for(int r=0;r<LIGNES;r++) for(int c=0;c<COLONNES;c++) if(b->cellules[r][c]==ch) cnt++; return cnt;
}

void Tableau_swap(Tableau *b, int r1,int c1,int r2,int c2) // Échange deux cellules du plateau
{
    char tmp = b->cellules[r1][c1];
    b->cellules[r1][c1] = b->cellules[r2][c2];
    b->cellules[r2][c2] = tmp;
}

//Trouve les séquences horizontales et verticales >=3,rectangles et formes en H
//Marque les cellules à supprimer et met à jour la grille , renvoie true si une suppression a eu lieu et incrémente les points
bool Tableau_trouver_et_supprimer_les_correspondances(Tableau *b, int *points)
{
    bool any_global = false;
    int points_accum = 0;

    //Répéter jusqu'à stabilisation
    while(1){
        bool remove[LIGNES][COLONNES];
        memset(remove,0,sizeof(remove));
        bool any=false;

        // horizontal
        for(int r=0;r<LIGNES;r++){
            int c=0;
            while(c<COLONNES){
                char base = b->cellules[r][c];
                int j=c+1;
                while(j<COLONNES && base!='\0' && b->cellules[r][j]==base) j++;
                int len = j-c;
                if(base!='\0' && len>=3){
                    any=true;
                    for(int k=c;k<j;k++) remove[r][k]=true;
                    if(len==4) points_accum += 4;
                    else if(len==6){
                        // remove all identical
                        char ch = base;
                        int total = Tableau_count_char(b,ch);
                        points_accum += total;
                        for(int rr=0;rr<LIGNES;rr++) for(int cc=0;cc<COLONNES;cc++) if(b->cellules[rr][cc]==ch) remove[rr][cc]=true;
                    } else {
                        points_accum += len;
                    }
                }
                c=j;
            }
        }

        // vertical
        for(int c=0;c<COLONNES;c++){
            int r=0;
            while(r<LIGNES){
                char base = b->cellules[r][c];
                int i=r+1;
                while(i<LIGNES && base!='\0' && b->cellules[i][c]==base) i++;
                int len = i-r;
                if(base!='\0' && len>=3){
                    any=true;
                    for(int k=r;k<i;k++) remove[k][c]=true;
                    if(len==4) points_accum += 4;
                    else if(len==6){
                        char ch = base;
                        int total = Tableau_count_char(b,ch);
                        points_accum += total;
                        for(int rr=0;rr<LIGNES;rr++) for(int cc=0;cc<COLONNES;cc++) if(b->cellules[rr][cc]==ch) remove[rr][cc]=true;
                    } else {
                        points_accum += len;
                    }
                }
                r=i;
            }
        }

        //détection de rectangles (brute-force) d'au moins 2x2
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                char ch = b->cellules[r][c];
                if(!ch) continue;
                for(int h=2;r+h-1<LIGNES;h++){
                    for(int w=2;c+w-1<COLONNES;w++){
                        bool ok=true;
                        for(int rr=r;rr<r+h && ok;rr++) for(int cc=c;cc<c+w;cc++) if(b->cellules[rr][cc]!=ch) {ok=false; break;}
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
        for(int r=0;r+2<LIGNES;r++){
            for(int c=0;c+2<COLONNES;c++){
                char ch = b->cellules[r][c];
                if(ch && b->cellules[r+1][c]==ch && b->cellules[r+2][c]==ch && b->cellules[r][c+2]==ch && b->cellules[r+1][c+2]==ch && b->cellules[r+2][c+2]==ch && b->cellules[r+1][c+1]==ch){
                    any=true;
                    for(int rr=r;rr<r+3;rr++) for(int cc=c;cc<c+3;cc++) remove[rr][cc]=true;
                    points_accum += 2*9; //2 * X où X = nombre d'ELEMENTS (ici 9)
                }
            }
        }

        if(!any) break; // plus de suppressions détectées

        any_global = true; // au moins une suppression globale a eu lieu
        //appliquer les suppressions
        for(int r=0;r<LIGNES;r++) for(int c=0;c<COLONNES;c++) if(remove[r][c]) b->cellules[r][c]='\0';
        Tableau_appliquer_gravité(b);
        //continuer  boucle pour détecter cascades
    }

    if(points) *points += points_accum;
    return any_global;
}

bool Tableau_pas_de_correspondances_initiales(Tableau *b)
{
    //vérifier séquences horizontales/verticales >= 3
    for(int r=0;r<LIGNES;r++){
        for(int c=0;c<COLONNES-2;c++){
            char ch = b->cellules[r][c]; if(ch==0) continue;
            if(b->cellules[r][c+1]==ch && b->cellules[r][c+2]==ch) return false;
        }
    }
    for(int c=0;c<COLONNES;c++){
        for(int r=0;r<LIGNES-2;r++){
            char ch = b->cellules[r][c]; if(ch==0) continue;
            if(b->cellules[r+1][c]==ch && b->cellules[r+2][c]==ch) return false;
        }
    }
    return true;
}

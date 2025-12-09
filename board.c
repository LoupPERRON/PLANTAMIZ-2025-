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

static int element_index(char ch)
{
    for(int i=0;i<NELEMENTS;i++) if(ELEMENTS[i]==ch) return i;
    return -1;
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
                b->cellules[r][c] = element_alea(); // Remplir chaque cellule avec un caractère aléatoire
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
                        b->cellules[r][c] = element_alea(); // rééchantillonner
                    }
                }
                // triple vertical commençant ici
                if(r <= LIGNES-3){
                    if(b->cellules[r][c] != '\0' && b->cellules[r][c] == b->cellules[r+1][c] && b->cellules[r][c] == b->cellules[r+2][c]){
                        b->cellules[r][c] = element_alea(); // rééchantillonner
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

void Tableau_print(Tableau *b, int curseur_l, int curseur_c, int choisi_l, int choisi_c)
{
    // Si le tampon de la console est trop petit pour positionner les caractères en sécurité, revenir à un affichage simple
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GetConsoleScreenBufferInfo(h, &csbi)){
        //impossible d'obtenir les informations de la console -> revenir à l'affichage ASCII
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                char ch = b->cellules[r][c]; if(ch=='\0') ch=' ';
                if(r==choisi_l && c==choisi_c) putchar(tolower(ch));
                else if(r==curseur_l && c==curseur_c) { putchar('['); putchar(ch); putchar(']'); }
                else putchar(ch);
            }
            putchar('\n'); // nouvelle ligne
        }
        fflush(stdout); // forcer la sortie
        return;
    } else {
        int largeur = csbi.dwSize.X; // largeur du tampon
        int hauteur = csbi.dwSize.Y; // hauteur du tampon
        if(largeur < COLONNES*2 || hauteur < LIGNES+6){
            //repli : retour à un affichage ASCII simple
            for(int r=0;r<LIGNES;r++){ // pour chaque ligne
                for(int c=0;c<COLONNES;c++){ // pour chaque colonne
                    char ch = b->cellules[r][c]; if(ch=='\0') ch=' '; // cellule vide
                    if(r==choisi_l && c==choisi_c) putchar(tolower(ch)); // sélectionné
                    else if(r==curseur_l && c==curseur_c) { putchar('['); putchar(ch); putchar(']'); } // curseur
                    else putchar(ch); // normal
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
            if(r==choisi_l && c==choisi_c){ 
                //sélectionné : caractère en arrière-plan lumineux
                Couleur(col, 4);
                putchar(tolower(ch));
                Couleur(15,0);
            } else if(r==curseur_l && c==curseur_c){
                //curseur : arrière-plan inversé
                Couleur(0, 14); // fond jaune
                putchar(ch); // afficher le caractère
                Couleur(15,0); // réinitialiser les couleurs
            } else {
                Couleur(col,0); // couleur normale
                putchar(ch); // afficher le caractère
                Couleur(15,0); // réinitialiser les couleurs
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
        int ecrire = LIGNES-1; // position d'écriture
        for(int r=LIGNES-1;r>=0;r--){ // parcourir de bas en haut
            if(b->cellules[r][c] != '\0'){ // si la cellule n'est pas vide
                b->cellules[ecrire][c] = b->cellules[r][c]; // déplacer l'élément vers la position d'écriture
                ecrire--; // déplacer la position d'écriture vers le haut
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
bool Tableau_trouver_et_supprimer_les_correspondances(Tableau *b, int *points, int *removed_counts, int delai_ms, int curseur_l, int curseur_c, int choisi_l, int choisi_c, int niveau, int coups_restants, int vies, int points_total, int progres[5], int targets[5])
{
    bool any_global = false;
    int points_accum = 0;
    int removed_accum[NELEMENTS];
    for(int i=0;i<NELEMENTS;i++) removed_accum[i]=0;

    // Raccourci : déléguer la détection à petites fonctions statiques pour lisibilité
    while(1){
        bool remove[LIGNES][COLONNES];
        memset(remove,0,sizeof(remove));
        bool any=false;

        // détecteurs
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

        // rectangles
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

        // H-shape
        for(int r=0;r+2<LIGNES;r++){
            for(int c=0;c+2<COLONNES;c++){
                char ch = b->cellules[r][c];
                if(ch && b->cellules[r+1][c]==ch && b->cellules[r+2][c]==ch && b->cellules[r][c+2]==ch && b->cellules[r+1][c+2]==ch && b->cellules[r+2][c+2]==ch && b->cellules[r+1][c+1]==ch){
                    any=true;
                    for(int rr=r;rr<r+3;rr++) for(int cc=c;cc<c+3;cc++) remove[rr][cc]=true;
                    points_accum += 2*9;
                }
            }
        }

        if(!any) break;

        any_global = true;
        // appliquer les suppressions et compter
        for(int r=0;r<LIGNES;r++){
            for(int c=0;c<COLONNES;c++){
                if(remove[r][c] && b->cellules[r][c] != '\0'){
                    int idx = element_index(b->cellules[r][c]);
                    if(idx >= 0) removed_accum[idx]++;
                    b->cellules[r][c] = '\0';
                }
            }
        }

        if(delai_ms > 0){
            // animate gravity in steps
            bool moved;
            do {
                moved = false;
                // shift down by one step where possible
                for(int c=0;c<COLONNES;c++){
                    for(int r=LIGNES-2;r>=0;r--){
                        if(b->cellules[r][c] != '\0' && b->cellules[r+1][c] == '\0'){
                            b->cellules[r+1][c] = b->cellules[r][c];
                            b->cellules[r][c] = '\0';
                            moved = true;
                        }
                    }
                }
                // fill top empties
                for(int c=0;c<COLONNES;c++){
                    if(b->cellules[0][c] == '\0'){
                        b->cellules[0][c] = element_alea();
                        moved = true;
                    }
                }
                // redraw header + board together so status is visible while animating
                effacer_ecran();
                // print header/status (same layout as afficher_statut)
                gotoligcol(0,0);
                printf("Niveau %d   Coups restants: %d   Vies: %d   Points: %d\n", niveau+1, coups_restants, vies, points_total);
                printf("Contrat: S:%d F:%d P:%d O:%d M:%d\n", targets[0], targets[1], targets[2], targets[3], targets[4]);
                printf("Progres: S:%d F:%d P:%d O:%d M:%d\n", progres[0], progres[1], progres[2], progres[3], progres[4]);
                Tableau_print(b, curseur_l, curseur_c, choisi_l, choisi_c);
                fflush(stdout);
                Sleep(delai_ms);
            } while(moved);
        } else {
            // comportement ancien : appliquer gravité complète sans animation
            Tableau_appliquer_gravité(b);
        }
    }

    if(points) *points += points_accum;
    if(removed_counts){
        for(int i=0;i<NELEMENTS;i++) removed_counts[i] = removed_accum[i];
    }
    return any_global;
}

bool Tableau_pas_de_correspondances_initiales(Tableau *b)
{
    //vérifier séquences horizontales/verticales >= 3
    for(int r=0;r<LIGNES;r++){
        for(int c=0;c<COLONNES-2;c++){
            char ch = b->cellules[r][c]; if(ch==0) continue; // cellule vide
            if(b->cellules[r][c+1]==ch && b->cellules[r][c+2]==ch) return false; // correspondance trouvée
        }
    }
    for(int c=0;c<COLONNES;c++){
        for(int r=0;r<LIGNES-2;r++){
            char ch = b->cellules[r][c]; if(ch==0) continue; // cellule vide
            if(b->cellules[r+1][c]==ch && b->cellules[r+2][c]==ch) return false; // correspondance trouvée
        }
    }
    return true;
}

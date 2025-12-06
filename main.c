#include <stdio.h>
#include <string.h>
#include "game.h"

static int menu()
{
    while(1){
        printf("Plantamitz - Menu\n");
        printf("1) Nouvelle partie\n");
        printf("2) Reprendre\n");
        printf("3) Quitter\n");
        printf("Choix: ");
        int c = getchar();
        // consume rest of line
        while(getchar()!='\n' && !feof(stdin));
        if(c=='1') return 1;
        if(c=='2') return 2;
        if(c=='3') return 3;
    }
}

int main()
{
    int choice = menu();
    if(choice==1){
        game_run(0); // new game
    } else if(choice==2){
        game_run(1); // resume
    } else {
        printf("Au revoir.\n");
    }
    return 0;
}

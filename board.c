#include "board.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "console.h"

static const char ITEMS[] = {'S','F','P','O','M'};
static const int NITEMS = 5;

static char rand_item()
{
    return ITEMS[rand() % NITEMS];
}

void board_init(Board *b)
{
    srand((unsigned)time(NULL));
    // Generate full boards up to MAX_ATTEMPTS until we get one without initial matches.
    // This avoids an unpredictable long loop that tweaks random cells endlessly.
    const int MAX_ATTEMPTS = 1000;
    int attempts = 0;
    do {
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                b->cells[r][c] = rand_item();
            }
        }
        attempts++;
        if(attempts >= MAX_ATTEMPTS) break;
    } while(!board_no_initial_matches(b));

    if(attempts >= MAX_ATTEMPTS){
        // Fallback: if we didn't find a clean board, break obvious triples by re-rolling offending cells.
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                // horizontal triple starting here
                if(c <= COLS-3){
                    if(b->cells[r][c] != '\0' && b->cells[r][c] == b->cells[r][c+1] && b->cells[r][c] == b->cells[r][c+2]){
                        b->cells[r][c] = rand_item();
                    }
                }
                // vertical triple starting here
                if(r <= ROWS-3){
                    if(b->cells[r][c] != '\0' && b->cells[r][c] == b->cells[r+1][c] && b->cells[r][c] == b->cells[r+2][c]){
                        b->cells[r][c] = rand_item();
                    }
                }
            }
        }
    }

    // debug: indicate attempts (helps diagnose hangs)
    // note: printing here is safe because caller prints debug messages already
    // but keep it non-mandatory; flush to ensure visibility when running.
    // printf("[DEBUG] board_init attempts=%d\n", attempts); fflush(stdout);
}

static int color_for_char(char ch)
{
    switch(ch){
        case 'S': return 14; // yellow
        case 'F': return 12; // red
        case 'P': return 10; // green
        case 'O': return 3;  // turquoise
        case 'M': return 6;  // brown/olive
        default: return 15;  // white
    }
}

void board_print(Board *b, int cursor_r, int cursor_c, int selected_r, int selected_c)
{
    // If console buffer is too small to position characters safely, fallback to plain print
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GetConsoleScreenBufferInfo(h, &csbi)){
        // couldn't get console info -> fallback ASCII print
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
            // fallback simple ascii print
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

    // top margin
    int base_row = 3;
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            int scr_r = base_row + r;
            int scr_c = c*2; // spacing
            gotoligcol(scr_r, scr_c);
            char ch = b->cells[r][c];
            if(ch == '\0') ch = ' ';
            int col = color_for_char(ch);
            if(r==selected_r && c==selected_c){
                // selected: lowercase and bright background
                Color(col, 4);
                putchar(tolower(ch));
                Color(15,0);
            } else if(r==cursor_r && c==cursor_c){
                // cursor: inverse background
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
    // move cursor after board
    gotoligcol(base_row+ROWS+1,0);
    fflush(stdout);
}

void board_apply_gravity(Board *b)
{
    for(int c=0;c<COLS;c++){
        int write = ROWS-1;
        for(int r=ROWS-1;r>=0;r--){
            if(b->cells[r][c] != '\0'){
                b->cells[write][c] = b->cells[r][c];
                write--;
            }
        }
        for(int r=write;r>=0;r--) b->cells[r][c] = rand_item();
    }
}

int board_count_char(Board *b,char ch)
{
    int cnt=0; for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(b->cells[r][c]==ch) cnt++; return cnt;
}

void board_swap(Board *b, int r1,int c1,int r2,int c2)
{
    char tmp = b->cells[r1][c1];
    b->cells[r1][c1] = b->cells[r2][c2];
    b->cells[r2][c2] = tmp;
}

// Find horizontal and vertical sequences >=3, rectangles and H-like shapes.
// Marks removals and updates board; returns true if any removal occurred and updates points.
bool board_find_and_remove_matches(Board *b, int *points)
{
    bool any_global = false;
    int points_accum = 0;

    // Repeat until stabilized
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

        // rectangle detection (simple brute-force) at least 2x2
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

        // H-shape detection: two vertical bars of 3 connected by middle horizontal (3x3 pattern)
        for(int r=0;r+2<ROWS;r++){
            for(int c=0;c+2<COLS;c++){
                char ch = b->cells[r][c];
                if(ch && b->cells[r+1][c]==ch && b->cells[r+2][c]==ch && b->cells[r][c+2]==ch && b->cells[r+1][c+2]==ch && b->cells[r+2][c+2]==ch && b->cells[r+1][c+1]==ch){
                    any=true;
                    for(int rr=r;rr<r+3;rr++) for(int cc=c;cc<c+3;cc++) remove[rr][cc]=true;
                    points_accum += 2*9; // 2 * X where X = number of items (here 9)
                }
            }
        }

        if(!any) break;

        any_global = true;
        // apply removals
        for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(remove[r][c]) b->cells[r][c]='\0';
        board_apply_gravity(b);
        // continue loop to detect cascades
    }

    if(points) *points += points_accum;
    return any_global;
}

bool board_no_initial_matches(Board *b)
{
    // check horizontal/vertical >=3
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

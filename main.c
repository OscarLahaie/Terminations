// Code principal de jeu en remplacement temporaire de termination.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void){
    int i, j;
    char hud[30][110];

    for(i = 0; i < 30; i++){
        for(j = 0; j < 110; j++){
            hud[i][j]   = ' ';
        }
    }

    for(i = 0; i < 30; i++){
        for(j = 0; j < 110; j++){
            hud[i][0]   = '#';
            hud[i][109] = '#';
            hud[0][j]   = '#';
            hud[29][j]  = '#';
        }
    }

//Play
    for(i = 5; i < 19; i++){
        for(j = 8; j < 48; j++){
            hud[i][7]    = '|';
            hud[i][48]   = '|';
            hud[4][j]    = '-';
            hud[19][j]   = '=';
        }
    }

    hud[7][16] = '#';

    hud[7][19] = '#';

    hud[7][30] = '#';
    hud[7][33] = '#';
    hud[8][29] = '#';

    for(i = 6; i < 18; i++){
        hud[i][10]    = '#';
    }

    for(j = 10; j < 16; j++){
        hud[6][j]    = '#';
    }
    for(j = 20; j < 23; j++){
        hud[6][j]    = '#';
    }
    for(j = 31; j < 33; j++){
        hud[6][j]    = '#';
    }
    for(j = 38; j < 40; j++){
        hud[6][j]    = '#';
    }
    for(j = 42; j < 44; j++){
        hud[6][j]    = '#';
    }

    for(i = 8; i < 12; i++){
        hud[i][17]    = '#';
        hud[i][19]    = '#';
    }

    for(i = 16; i <= 17; i++){
        for(j = 10; j < 15; j++){
            hud[i][j] = '#';
        }
        for(j = 21; j < 27; j++){
            hud[i][j] = '#';
        }
        for(j = 28; j < 31; j++){
            hud[i][j] = '#';
        }
        for(j = 32; j < 36; j++){
            hud[i][j] = '#';
        }
        for(j = 38; j < 41; j++){
            hud[i][j] = '#';
        }
    }

    for(j = 13; j < 15; j++){
        hud[9][j] = '#';
    }
    for(j = 40; j < 42; j++){
        hud[7][j] = '#';
    }

    for(i = 12; i < 16; i++){
        hud[i][14] = '#';
    }

    for(i = 12; i < 14; i++){
        hud[i][15] = '#';
    }
    for(i = 11; i < 13; i++){
        hud[i][16] = '#';
    }

    for(i = 11; i < 16; i++){
        hud[i][20] = '#';
    }
    for(i = 7; i < 13; i++){
        hud[i][23] = '#';
    }
    for(i = 13; i < 16; i++){
        hud[i][26] = '#';
    }
    for(j = 24; j < 26; j++){
        hud[12][j] = '#';
    }

    for(i = 9; i < 16; i++){
        hud[i][28] = '#';
    }
    for(i = 10; i < 16; i++){
        hud[i][35] = '#';
    }
    for(i = 8; i < 10; i++){
        hud[i][34] = '#';
    }
    for(i = 10; i < 12; i++){
        hud[i][31] = '#';
    }
    for(i = 14; i < 16; i++){
        for(j = 31; j < 33; j++){
            hud[i][j] = '#';
        }
    }

    for(i = 7; i < 11; i++){
        hud[i][37] = '#';
        hud[i][44] = '#';
    }
    for(i = 10; i < 12; i++){
        hud[i][38] = '#';
    }
    for(i = 11; i < 13; i++){
        hud[i][39] = '#';
    }
    for(i = 13; i < 16; i++){
        hud[i][38] = '#';
    }
    for(i = 10; i < 13; i++){
        hud[i][43] = '#';
    }
    for(i = 12; i < 15; i++){
        hud[i][42] = '#';
    }
    for(i = 14; i < 17; i++){
        hud[i][41] = '#';
    }
    






//Exit
    for(i = 5; i < 19; i++){
        for(j = 62; j < 102; j++){
            hud[i][61]    = '|';
            hud[i][102]   = '|';
            hud[4][j]    = '-';
            hud[19][j]   = '=';
        }
    }




//Affichage du HUD
    for(i = 0; i < 30; i++){
        for(j = 0; j < 110; j++){
            printf("%c", hud[i][j]);
        }
        printf("\n");
    }
}
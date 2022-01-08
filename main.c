// Code principal de jeu en remplacement temporaire de termination.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "map.h"
#define HEIGHT_MAX 50
#define WIDTH_MAX 50

int taille_map = 25;
int difficulte = 0;
int map[HEIGHT_MAX][WIDTH_MAX];
// Retourne 0 si le joueur choisit de jouer, 1 si il désire quitter, 2 pour les paramètres
int afficher_menu()
{
    system("clear");
    int i, j;
    char hud[30][110];

    for (i = 0; i < 30; i++)
    {
        for (j = 0; j < 110; j++)
        {
            hud[i][j] = ' ';
        }
    }

    for (i = 0; i < 30; i++)
    {
        for (j = 0; j < 110; j++)
        {
            hud[i][0] = '#';
            hud[i][109] = '#';
            hud[0][j] = '#';
            hud[29][j] = '#';
        }
    }

    char termination[] = "TERMINATION";
    char hudfooter[] = "Que voulez vous faire ? (Play/Exit/Parametres) ";

    for (j = 49; j < 60; j++)
    {
        hud[1][j] = termination[j - 49];
    }

    for (j = 32; j < 79; j++)
    {
        hud[28][j] = hudfooter[j - 32];
    }

    //Play
    for (i = 5; i < 19; i++)
    {
        for (j = 8; j < 48; j++)
        {
            hud[i][7] = '|';
            hud[i][48] = '|';
            hud[4][j] = '-';
            hud[19][j] = '=';
        }
    }

    hud[7][16] = '#';
    hud[7][19] = '#';
    hud[7][30] = '#';
    hud[7][33] = '#';
    hud[8][29] = '#';

    for (i = 6; i < 18; i++)
    {
        hud[i][10] = '#';
    }
    for (j = 10; j < 16; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 20; j < 23; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 31; j < 33; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 38; j < 40; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 42; j < 44; j++)
    {
        hud[6][j] = '#';
    }
    for (i = 8; i < 12; i++)
    {
        hud[i][17] = '#';
        hud[i][19] = '#';
    }
    for (i = 16; i <= 17; i++)
    {
        for (j = 10; j < 15; j++)
        {
            hud[i][j] = '#';
        }
        for (j = 21; j < 27; j++)
        {
            hud[i][j] = '#';
        }
        for (j = 28; j < 31; j++)
        {
            hud[i][j] = '#';
        }
        for (j = 32; j < 36; j++)
        {
            hud[i][j] = '#';
        }
        for (j = 38; j < 41; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (j = 13; j < 15; j++)
    {
        hud[9][j] = '#';
    }
    for (j = 40; j < 42; j++)
    {
        hud[7][j] = '#';
    }

    for (i = 12; i < 16; i++)
    {
        hud[i][14] = '#';
    }

    for (i = 12; i < 14; i++)
    {
        hud[i][15] = '#';
    }
    for (i = 11; i < 13; i++)
    {
        hud[i][16] = '#';
    }
    for (i = 11; i < 16; i++)
    {
        hud[i][20] = '#';
    }
    for (i = 7; i < 13; i++)
    {
        hud[i][23] = '#';
    }
    for (i = 13; i < 16; i++)
    {
        hud[i][26] = '#';
    }
    for (j = 24; j < 26; j++)
    {
        hud[12][j] = '#';
    }

    for (i = 9; i < 16; i++)
    {
        hud[i][28] = '#';
    }
    for (i = 10; i < 16; i++)
    {
        hud[i][35] = '#';
    }
    for (i = 8; i < 10; i++)
    {
        hud[i][34] = '#';
    }
    for (i = 10; i < 12; i++)
    {
        hud[i][31] = '#';
    }
    for (i = 14; i < 16; i++)
    {
        for (j = 31; j < 33; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (i = 7; i < 11; i++)
    {
        hud[i][37] = '#';
        hud[i][44] = '#';
    }
    for (i = 10; i < 12; i++)
    {
        hud[i][38] = '#';
    }
    for (i = 11; i < 13; i++)
    {
        hud[i][39] = '#';
    }
    for (i = 13; i < 16; i++)
    {
        hud[i][38] = '#';
    }
    for (i = 10; i < 13; i++)
    {
        hud[i][43] = '#';
    }
    for (i = 12; i < 15; i++)
    {
        hud[i][42] = '#';
    }
    for (i = 14; i < 17; i++)
    {
        hud[i][41] = '#';
    }

    //Exit
    for (i = 5; i < 19; i++)
    {
        for (j = 62; j < 102; j++)
        {
            hud[i][61] = '|';
            hud[i][102] = '|';
            hud[4][j] = '-';
            hud[19][j] = '=';
        }
    }

    hud[9][69] = '#';

    for (j = 65; j < 70; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 65; j < 72; j++)
    {
        hud[17][j] = '#';
    }
    for (j = 65; j < 73; j++)
    {
        hud[16][j] = '#';
    }
    for (i = 7; i < 16; i++)
    {
        hud[i][65] = '#';
    }
    for (i = 13; i < 16; i++)
    {
        hud[i][72] = '#';
    }
    for (i = 7; i < 11; i++)
    {
        hud[i][71] = '#';
    }
    for (i = 9; i < 12; i++)
    {
        hud[i][70] = '#';
    }
    for (j = 69; j < 72; j++)
    {
        hud[12][j] = '#';
    }

    for (i = 6; i < 11; i++)
    {
        hud[i][74] = '#';
        hud[i][81] = '#';
    }
    for (i = 16; i < 18; i++)
    {
        for (j = 74; j < 77; j++)
        {
            hud[i][j] = '#';
        }
        for (j = 79; j < 82; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (i = 15; i < 17; i++)
    {
        for (j = 77; j < 79; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (i = 13; i < 17; i++)
    {
        hud[i][74] = '#';
        hud[i][81] = '#';
    }
    for (i = 10; i < 13; i++)
    {
        hud[i][75] = '#';
        hud[i][80] = '#';
    }
    for (j = 75; j < 77; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 79; j < 81; j++)
    {
        hud[6][j] = '#';
    }
    for (j = 77; j < 79; j++)
    {
        hud[7][j] = '#';
    }

    for (i = 7; i < 13; i++)
    {
        hud[i][83] = '#';
    }
    for (i = 12; i < 18; i++)
    {
        hud[i][84] = '#';
    }
    for (i = 6; i < 18; i++)
    {
        hud[i][88] = '#';
    }
    for (i = 16; i < 18; i++)
    {
        for (j = 84; j < 88; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (j = 84; j < 88; j++)
    {
        hud[6][j] = '#';
    }

    for (j = 90; j < 98; j++)
    {
        hud[6][j] = '#';
    }
    for (i = 16; i < 18; i++)
    {
        for (j = 93; j < 97; j++)
        {
            hud[i][j] = '#';
        }
    }
    for (i = 7; i < 11; i++)
    {
        hud[i][90] = '#';
    }
    for (i = 10; i < 12; i++)
    {
        hud[i][91] = '#';
        hud[i][95] = '#';
    }
    for (i = 7; i < 11; i++)
    {
        hud[i][97] = '#';
    }
    for (i = 10; i < 17; i++)
    {
        hud[i][92] = '#';
        hud[i][96] = '#';
    }

    //Paramètres
    for (i = 22; i < 26; i++)
    {
        for (j = 30; j < 32; j++)
        {
            hud[i][j] = '*';
            hud[i][j + 5] = '*';
            hud[i][j + 10] = '*';
            hud[i][j + 15] = '*';
            hud[i][j + 26] = '*';
            hud[i][j + 32] = '*';
            hud[i][j + 36] = '*';
            hud[i][j + 41] = '*';
        }
    }
    for (i = 22; i < 26; i++)
    {
        hud[i][38] = '*';
        hud[i][48] = '*';
        hud[i][50] = '*';
        hud[i][54] = '*';
    }
    for (j = 32; j < 34; j++)
    {
        hud[22][j] = '*';
        hud[24][j] = '*';
        hud[22][j + 5] = '*';
        hud[24][j + 5] = '*';
        hud[22][j + 15] = '*';
        hud[24][j + 15] = '*';
        hud[22][j + 26] = '*';
        hud[25][j + 26] = '*';
        hud[22][j + 41] = '*';
        hud[25][j + 41] = '*';
        hud[25][j + 44] = '*';
        hud[22][j + 46] = '*';
    }
    for (i = 22; i < 24; i++)
    {
        hud[i][51] = '*';
        hud[i + 1][52] = '*';
        hud[i][53] = '*';
        hud[i][76] = '*';
        hud[i][77] = '*';
        hud[i + 2][78] = '*';
        hud[i + 2][79] = '*';
    }
    hud[23][33] = '*';
    hud[22][42] = '*';
    hud[24][42] = '*';
    hud[23][43] = '*';
    hud[25][43] = '*';
    hud[23][58] = '*';
    hud[22][61] = '*';
    hud[22][64] = '*';
    hud[22][68] = '*';
    hud[24][68] = '*';
    hud[23][69] = '*';
    hud[25][69] = '*';
    hud[23][73] = '*';

    //Affichage du HUD
    for (i = 0; i < 30; i++)
    {
        for (j = 0; j < 110; j++)
        {
            printf("%c", hud[i][j]);
        }
        printf("\n");
    }

    char phrase[100];
    printf("Votre choix : ");
    scanf("%s", phrase);
    if (strcmp(phrase, "play") == 0 || strcmp(phrase, "Play") == 0 || strcmp(phrase, "PLAY") == 0)
    {
        return 0;
    }
    else if (strcmp(phrase, "exit") == 0 || strcmp(phrase, "Exit") == 0 || strcmp(phrase, "EXIT") == 0)
    {
        return 1;
    }
    else if (strcmp(phrase, "paramètres") == 0 || strcmp(phrase, "Paramètres") == 0 || strcmp(phrase, "PARAMETRES") == 0 || strcmp(phrase, "parametres") == 0)
    {
        return 2;
    }
    else
    {
        return afficher_menu();
    }
}

void afficher_parametres(int selection)
{
    printf("\033[40m");
    printf("\033[37m");
    printf("\rFleches du haut et du bas pour se déplacer et entrer pour selectionner\n\n");
    printf("\rParamètres :\n\n");
    printf("\rTaille de la carte :\n");
    for (int i = 0; i < 5; i++)
    {

        if (i == selection && i * 5 + 20 == taille_map)
        {
            printf("\033[47m");
            printf("\033[31m");
            printf("\rtaille : %d*%d\n", i * 5 + 20, i * 5 + 20);
        }
        else if (i == selection)
        {
            printf("\033[47m");
            printf("\033[30m");
            printf("\rtaille : %d*%d\n", i * 5 + 20, i * 5 + 20);
        }
        else if (i * 5 + 20 == taille_map)
        {
            printf("\033[41m");
            printf("\033[37m");
            printf("\rtaille : %d*%d\n", i * 5 + 20, i * 5 + 20);
        }
        else
        {
            printf("\033[40m");
            printf("\033[37m");
            printf("\rtaille : %d*%d\n", i * 5 + 20, i * 5 + 20);
        }
    }
    printf("\033[40m");
    printf("\033[37m");
    printf("\n\rDifficulté :\n");
    char phrase[3][10] = {{"Facile"}, {"Medium"}, {"Dur"}};
    for (int i = 0; i < 3; i++)
    {

        if (i + 5 == selection && i == difficulte)
        {
            printf("\033[47m");
            printf("\033[31m");
            printf("\rniveau : %s\n", phrase[i]);
        }
        else if (i + 5 == selection)
        {
            printf("\033[47m");
            printf("\033[30m");
            printf("\rniveau : %s\n", phrase[i]);
        }
        else if (i == difficulte)
        {
            printf("\033[41m");
            printf("\033[37m");
            printf("\rniveau : %s\n", phrase[i]);
        }
        else
        {
            printf("\033[40m");
            printf("\033[37m");
            printf("\rniveau : %s\n", phrase[i]);
        }
    }
    if (selection == 8)
    {
        printf("\033[47m");
        printf("\033[30m");
    }
    else
    {
        printf("\033[40m");
        printf("\033[37m");
    }
    printf("\n\rExit");

    printf("\r");
    printf("\033[0m");
}

int main(void)
{
    system("reset");
    while (1)
    {
        int choix = afficher_menu();
        if (choix == 0)
        {
            system("clear");
            srand(time(NULL));

            classique(map);
            afficher(map, 0);
            //liaison avec le jeu
        }
        else if (choix == 1)
        {
            system("clear");
            printf("Merci d'avoir joué ! \n");
            break;
        }
        else if (choix == 2)
        {
            int c;
            int selection = 0;
            system("clear");
            system("/usr/bin/stty raw");
            while ((c = getchar()) != 'q')
            {
                printf("\r\033[2K");
                system("clear");
                switch (c)
                {
                case 'B':
                    if (selection <= 8)
                        selection++;
                    break;
                case 'A':
                    if (selection >= 0)
                        selection--;
                    break;
                case ' ':
                    if (selection <= 4)
                    {
                        taille_map = 20 + selection * 5;
                    }
                    else if (selection <= 7)
                    {
                        difficulte = selection - 5;
                    }
                    break;
                }
                if (selection == 8 && c == ' ')
                {
                    system("reset");
                    break;
                }
                afficher_parametres(selection);
            }
        }
    }
}
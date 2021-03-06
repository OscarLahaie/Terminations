#ifndef _map_h_
#define _map_h_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define HEIGHT_MAX 50
#define WIDTH_MAX 50
#ifndef COORD
#define COORD
typedef struct Coordonnees Coordonnees;
struct Coordonnees
{
    int x;
    int y;
};
#endif
#ifndef UNITE
#define UNITE
typedef struct Unites Unites;
struct Unites
{
    int position_x;
    int position_y;
    int deplacement_x;
    int deplacement_y;
    int type;
    int equipe;
};
#endif
void afficher(int map[HEIGHT_MAX][WIDTH_MAX], int map_unite[HEIGHT_MAX][WIDTH_MAX], int type, int taille, Coordonnees selection, bool etat_move)
{
    if (type == 0)
    {
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en prairie
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                // Couleur du fond
                if (selection.x == ligne && selection.y == colonne)
                {
                    if (etat_move)
                    {
                        printf("\033[4;5;213m");
                    }
                    else
                    {
                        printf("\033[105m");
                    }
                }
                else if (map[colonne][ligne] < 0)
                {
                    //Bleu foncé pour le fond de l'eau
                    printf("\033[0;44m");
                }
                else if (map[colonne][ligne] == 0)
                {
                    // Bleu clair pour les bords
                    printf("\033[0;46m");
                }
                else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2)
                {
                    // Vert pour les prairies et les arbres
                    printf("\033[0;42m");
                }
                else if (map[colonne][ligne] <= 5)
                {
                    printf("\033[0;43m");
                }
                else if (map[colonne][ligne] == 99)
                {
                    printf("\033[101m");
                }
                //chateau
                else if (map[colonne][ligne] == 100)
                {
                    printf("\033[104m");
                }
                else if (map[colonne][ligne] == 101)
                {
                    printf("\033[100m");
                }
                //event
                else if (map[colonne][ligne] == 216)
                {
                    printf("\033[41m");
                }
                else
                {
                    // Blanc pour les montagnes enneigées
                    printf("\033[0;47m");
                }
                // Fin couleur du fond

                // Couleur + caractères
                if (map_unite[colonne][ligne] == 0)
                {
                    if (map[colonne][ligne] == 2)
                    {
                        printf("\033[2;32m");
                        printf("XX");
                    }
                    else if (map[colonne][ligne] > 5 && map[colonne][ligne] <= 10)
                    {
                        printf("\033[2;30m");
                        printf("MM");
                    }
                    else if (map[colonne][ligne] == 101)
                    {
                        printf("\033[30m");
                        printf("¤¤");
                    }
                    else if (map[colonne][ligne] == 216)
                    {
                        printf("\033[37m");
                        printf("$$");
                    }
                    else
                    {
                        printf("  ");
                    }
                }
                else
                {
                    //Equipe rouge
                    if (map_unite[colonne][ligne] == 1)
                    {
                        printf("\033[38;5;124m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 2)
                    {
                        printf("\033[38;5;124m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 3)
                    {
                        printf("\033[38;5;124m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 4)
                    {
                        printf("\033[38;5;124m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 5)
                    {
                        printf("\033[38;5;124m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 6)
                    {
                        printf("\033[38;5;124m");
                        printf("o/");
                    }

                    //Equipe bleue
                    else if (map_unite[colonne][ligne] == 11)
                    {
                        printf("\033[38;5;117m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 12)
                    {
                        printf("\033[38;5;117m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 13)
                    {
                        printf("\033[38;5;117m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 14)
                    {
                        printf("\033[38;5;117m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 15)
                    {
                        printf("\033[38;5;117m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 16)
                    {
                        printf("\033[38;5;117m");
                        printf("o/");
                    }

                    else
                    {
                        printf("\033[30m");
                        printf("  ");
                    }
                }

                // Fin couleur + caractères
                printf("\033[0m");
            }
            printf("\033[0m \n\r");
        }
        printf("\033[0m\n\r");
    }
    else if (type == 1)
    {
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en glace
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (selection.x == ligne && selection.y == colonne)
                {
                    if (etat_move)
                    {
                        printf("\033[4;5;42m");
                    }
                    else
                    {
                        printf("\033[105m");
                    }
                }
                // Couleur du fond
                else if (map[colonne][ligne] < 0)
                {
                    //Bleu foncé pour le fond de l'eau
                    printf("\033[0;44m");
                }
                else if (map[colonne][ligne] == 0)
                {
                    // Bleu clair pour les bords
                    printf("\033[0;46m");
                }
                else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2)
                {
                    // blanc neige
                    printf("\033[0;47m");
                }
                else if (map[colonne][ligne] == 99)
                {
                    printf("\033[101m");
                }
                else if (map[colonne][ligne] == 100)
                {
                    printf("\033[104m");
                }
                else if (map[colonne][ligne] == 101)
                {
                    printf("\033[100m");
                }
                else if (map[colonne][ligne] == 216)
                {
                    printf("\033[41m");
                }
                // Fin couleur du fond
                // Couleur + caractères
                if (map_unite[colonne][ligne] == 0)
                {
                    if (map[colonne][ligne] == 2)
                    {
                        printf("\033[2;30m");
                        printf("XX");
                    }
                    else if (map[colonne][ligne] <= 0)
                    {
                        printf("\033[2;37m");
                        printf("//");
                    }
                    else if (map[colonne][ligne] == 101)
                    {
                        printf("\033[30m");
                        printf("¤¤");
                    }
                    else if (map[colonne][ligne] == 216)
                    {
                        printf("\033[37m");
                        printf("$$");
                    }
                    else
                    {
                        printf("  ");
                    }
                }
                else
                {
                    //Equipe rouge
                    if (map_unite[colonne][ligne] == 1)
                    {
                        printf("\033[38;5;124m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 2)
                    {
                        printf("\033[38;5;124m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 3)
                    {
                        printf("\033[38;5;124m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 4)
                    {
                        printf("\033[38;5;124m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 5)
                    {
                        printf("\033[38;5;124m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 6)
                    {
                        printf("\033[38;5;124m");
                        printf("o/");
                    }

                    //Equipe bleue
                    else if (map_unite[colonne][ligne] == 11)
                    {
                        printf("\033[38;5;117m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 12)
                    {
                        printf("\033[38;5;117m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 13)
                    {
                        printf("\033[38;5;117m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 14)
                    {
                        printf("\033[38;5;117m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 15)
                    {
                        printf("\033[38;5;117m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 16)
                    {
                        printf("\033[38;5;117m");
                        printf("o/");
                    }

                    else
                    {
                        printf("\033[30m");
                        printf("  ");
                    }
                }
                // Fin couleur + caractères
            }
            printf("\033[0m \n\r");
        }
        printf("\033[0m\n\r");
    }
    else if (type == 2)
    {
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en glace
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (selection.x == ligne && selection.y == colonne)
                {
                    if (etat_move)
                    {
                        printf("\033[4;5;47m");
                    }
                    else
                    {
                        printf("\033[105m");
                    }
                }
                // Couleur du fond
                else if (map[colonne][ligne] < 0)
                {
                    //Bleu clair oasis
                    printf("\033[0;46m");
                }
                else if (map[colonne][ligne] == 0)
                {
                    // Vert pour le contour luxuriant
                    printf("\033[0;42m");
                }
                else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2)
                {
                    // désert jaune
                    printf("\033[0;43m");
                }
                else if (map[colonne][ligne] == 99)
                {
                    printf("\033[101m");
                }
                else if (map[colonne][ligne] == 100)
                {
                    printf("\033[104m");
                }
                else if (map[colonne][ligne] == 101)
                {
                    printf("\033[100m");
                }
                else if (map[colonne][ligne] == 216)
                {
                    printf("\033[41m");
                }
                // Fin couleur du fond
                // Couleur + caractères
                if (map_unite[colonne][ligne] == 0)
                {
                    if (map[colonne][ligne] == 2)
                    {
                        printf("\033[2;32m");
                        printf("##");
                    }
                    else if (map[colonne][ligne] == 0)
                    {
                        printf("\033[2;32m");
                        printf("XX");
                    }
                    else if (map[colonne][ligne] == 101)
                    {
                        printf("\033[30m");
                        printf("¤¤");
                    }
                    else if (map[colonne][ligne] == 216)
                    {
                        printf("\033[37m");
                        printf("$$");
                    }
                    else
                    {
                        printf("  ");
                    }
                }
                else
                {
                    //Equipe rouge
                    if (map_unite[colonne][ligne] == 1)
                    {
                        printf("\033[38;5;124m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 2)
                    {
                        printf("\033[38;5;124m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 3)
                    {
                        printf("\033[38;5;124m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 4)
                    {
                        printf("\033[38;5;124m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 5)
                    {
                        printf("\033[38;5;124m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 6)
                    {
                        printf("\033[38;5;124m");
                        printf("o/");
                    }

                    //Equipe bleue
                    else if (map_unite[colonne][ligne] == 11)
                    {
                        printf("\033[38;5;117m");
                        printf(":o");
                    }
                    else if (map_unite[colonne][ligne] == 12)
                    {
                        printf("\033[38;5;117m");
                        printf(":v");
                    }
                    else if (map_unite[colonne][ligne] == 13)
                    {
                        printf("\033[38;5;117m");
                        printf("B)");
                    }
                    else if (map_unite[colonne][ligne] == 14)
                    {
                        printf("\033[38;5;117m");
                        printf(":)");
                    }
                    else if (map_unite[colonne][ligne] == 15)
                    {
                        printf("\033[38;5;117m");
                        printf(";)");
                    }
                    else if (map_unite[colonne][ligne] == 16)
                    {
                        printf("\033[38;5;117m");
                        printf("o/");
                    }

                    else
                    {
                        printf("\033[30m");
                        printf("  ");
                    }
                }
                // Fin couleur + caractères
            }
            printf("\033[0m \n\r");
        }
        printf("\033[0m\n\r");
    }
}

void classique(int map[HEIGHT_MAX][WIDTH_MAX], int taille)
{
    // Mise aléatoire de l'eau
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 1000 < 1)
            {
                map[colonne][ligne] = -1 * (rand() % 10);
            }
            else
            {
                map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);

    for (int i = 0; i < 5; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] < 0)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de la mise en place de l'eau

    // Mise en place des montagnes

    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 500 < 1 && map[colonne][ligne] == 1)
            {
                map[colonne][ligne] = 10 + rand() % 20;
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] > 10)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = 10 + (rand() % (map[colonne][ligne] - 10));
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = 10 + (rand() % (map[colonne][ligne] - 10));
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = 10 + (rand() % (map[colonne][ligne] - 10));
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = 10 + (rand() % (map[colonne][ligne] - 10));
                    }
                }
            }
        }
    }

    // Fin de la mise en place des montagnes

    // Mise en place des arbres

    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 1000 < 1 && map[colonne][ligne] == 1)
            {
                map[colonne][ligne] = 1 + rand() % 5;
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] > 1 && map[colonne][ligne] < 10)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = 1 + (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }

    //Fin de la mise en place des arbres

    //Harmonisation des reliefs
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (map[colonne][ligne] < 10 && map[colonne][ligne] >= 2)
            {
                map[colonne][ligne] = 2;
            }
        }
    }
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (map[colonne][ligne] >= 10)
            {
                map[colonne][ligne] -= 6;
            }
        }
    }
}

void toundra(int map[HEIGHT_MAX][WIDTH_MAX], int taille)
{

    // Mise en place de la glace
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 1000 < 1)
            {
                map[colonne][ligne] = -1 * (rand() % 10);
            }
            else
            {
                map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);

    for (int i = 0; i < 5; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] < 0)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de la glace

    // Mise en place des arbres
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 1000 < 1 && map[colonne][ligne] == 1)
            {
                map[colonne][ligne] = 1 + rand() % 5;
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] > 1 && map[colonne][ligne] < 10)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = 1 + (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin des arbres
}

void desert(int map[HEIGHT_MAX][WIDTH_MAX], int taille)
{

    // Mise en place des oasis
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 1000 < 1)
            {
                map[colonne][ligne] = -1 * (rand() % 10);
            }
            else
            {
                map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);

    for (int i = 0; i < 5; i++)
    {
        for (int colonne = 0; colonne < taille; colonne++)
        {
            for (int ligne = 0; ligne < taille; ligne++)
            {
                if (map[colonne][ligne] < 0)
                {
                    if (colonne - 1 >= 0 && map[colonne - 1][ligne] == 1)
                    {
                        map[colonne - 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne + 1 <= taille && map[colonne + 1][ligne] == 1)
                    {
                        map[colonne + 1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne - 1] == 1)
                    {
                        map[colonne][ligne - 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= taille && map[colonne][ligne + 1] == 1)
                    {
                        map[colonne][ligne + 1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de oasis

    // Mise en place des cactus
    for (int colonne = 0; colonne < taille; colonne++)
    {
        for (int ligne = 0; ligne < taille; ligne++)
        {
            if (rand() % 100 < 1 && map[colonne][ligne] == 1)
            {
                map[colonne][ligne] = 2;
            }
        }
    }

    // Fin des cactus
}

void events(int map[HEIGHT_MAX][WIDTH_MAX], int taille, int nb_events)
{
    int init = 1;
    while (init <= nb_events)
    {
        int random_line = rand() % (taille + 1);
        int random_column = rand() % (taille + 1);

        if (map[random_line][random_column] == 1)
        {
            map[random_line][random_column] = 216;
            init++;
        }
    }
}

void chateaux(int map[HEIGHT_MAX][WIDTH_MAX], int taille)
{
    for (int ligne = 1; ligne <= 3; ligne++)
    {
        for (int colonne = taille / 2 - 1; colonne <= taille / 2 + 1; colonne++)
        {
            if ((ligne != 1 && ligne != 3) && (colonne != taille / 2 - 1 && colonne != taille / 2 + 1))
            {
                map[ligne][colonne] = 100;
                map[taille - ligne - 1][colonne] = 99;
            }
            else
            {
                map[ligne][colonne] = 101;
                map[taille - ligne - 1][colonne] = 101;
            }
        }
    }
}

#endif
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 50
#define HEIGHT 50
#define BALAYAGE 100

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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
// Création d'un nouveau type de variable pour créer des tableau stockant le trajet à parcourir une fois le chemin trouvé

void convert(int map[HEIGHT][WIDTH], int map_acces[HEIGHT][WIDTH], int taille)
{
    for (int i = 0; i < taille; i++)
    {
        for (int j = 0; j < taille; j++)
        {
            map_acces[i][j] = (map[i][j] >= 0 && map[i][j] < 5) || map[i][j] == 100 || map[i][j] == 101 ? 0 : -1;
        }
    }
}

void recherche_inverse(int map[HEIGHT][WIDTH], Coordonnees chemin[HEIGHT * WIDTH], Coordonnees depart, Coordonnees arrivee)
{
    if (depart.x == arrivee.x && depart.y == arrivee.y)
    {
        chemin[0] = depart;
    }
    else
    {
        int min = 0;
        Coordonnees min_coord;
        if (map[arrivee.y - 1][arrivee.x] > 0)
        {

            min = map[arrivee.y - 1][arrivee.x];
            min_coord.y = arrivee.y - 1;
            min_coord.x = arrivee.x;
        }
        if (map[arrivee.y + 1][arrivee.x] > 0 && (map[arrivee.y + 1][arrivee.x] < min || min == 0))
        {
            min = map[arrivee.y + 1][arrivee.x];
            min_coord.y = arrivee.y + 1;
            min_coord.x = arrivee.x;
        }
        if (map[arrivee.y][arrivee.x - 1] > 0 && (map[arrivee.y][arrivee.x - 1] < min || min == 0))
        {
            min = map[arrivee.y][arrivee.x - 1];
            min_coord.y = arrivee.y;
            min_coord.x = arrivee.x - 1;
        }
        if (map[arrivee.y][arrivee.x + 1] > 0 && (map[arrivee.y][arrivee.x + 1] < min || min == 0))
        {
            min = map[arrivee.y][arrivee.x + 1];
            min_coord.y = arrivee.y;
            min_coord.x = arrivee.x + 1;
        }
        chemin[min - 1] = min_coord;
        chemin[min] = arrivee;
        recherche_inverse(map, chemin, depart, min_coord);
    }
}

void chemins(int map[HEIGHT][WIDTH], Coordonnees chemin[HEIGHT * WIDTH], Coordonnees depart, Coordonnees arrivee, int taille)
{
    int recherche[HEIGHT][WIDTH] = {0};
    //Marquage du départ et de l'arrivée sur la map par 1 pour le départ et -2 pour l'arrivée
    for (int i = 0; i < taille; i++)
    {
        for (int j = 0; j < taille; j++)
        {
            if (i == depart.y && j == depart.x)
            {
                map[i][j] = 1;
            }
            if (i == arrivee.y && j == arrivee.x)
            {
                map[i][j] = -2;
            }
        }
    }
    bool est_trouvee = false;
    // Variable pour dire si on trouve l'arrivée si on ne la trouve pas on renverra x = - 1 et y = -1 en premier element du tableau chemin
    for (int nombre = 0; nombre < BALAYAGE && !est_trouvee; nombre++)
    {
        for (int i = 0; i < taille && !est_trouvee; i++)
        {
            for (int j = 0; j < taille && !est_trouvee; j++)
            {
                if (map[i][j] >= 1)
                {
                    if (map[i - 1][j] == -2 || map[i + 1][j] == -2 || map[i][j - 1] == -2 || map[i][j + 1] == -2)
                    {
                        est_trouvee = true;
                        break;
                    }
                    if (i > 0 && map[i - 1][j] == 0)
                    {
                        recherche[i - 1][j] = map[i][j] + 1;
                    }
                    if (taille - 1 > i && map[i + 1][j] == 0)
                    {
                        recherche[i + 1][j] = map[i][j] + 1;
                    }
                    if (j > 0 && map[i][j - 1] == 0)
                    {
                        recherche[i][j - 1] = map[i][j] + 1;
                    }
                    if (taille - 1 > j && map[i][j + 1] == 0)
                    {
                        recherche[i][j + 1] = map[i][j] + 1;
                    }
                }
            }
        }

        for (int i = 0; i < taille; i++)
        {
            for (int j = 0; j < taille; j++)
            {
                if (map[i][j] == 0)
                {
                    map[i][j] = recherche[i][j];
                }
            }
        }
    }
    if (est_trouvee)
    {
        recherche_inverse(map, chemin, depart, arrivee);
    }
    else
    {
        Coordonnees fausse;
        fausse.x = -1;
        fausse.y = -1;
        chemin[0] = fausse;
    }
}

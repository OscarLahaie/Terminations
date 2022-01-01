#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 25
#define HEIGHT 25
#define BALAYAGE 100

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Création d'un nouveau type de variable pour créer des tableau stockant le trajet à parcourir une fois le chemin trouvé
typedef struct Coordonnees Coordonnees;
struct Coordonnees
{
    int x;
    int y;
};

void maptest(int map[HEIGHT][WIDTH])
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            map[i][j] = (rand() % 5) == 0 ? -1 : 0;
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

void chemins(int map[HEIGHT][WIDTH], Coordonnees chemin[HEIGHT * WIDTH], Coordonnees depart, Coordonnees arrivee)
{
    int recherche[HEIGHT][WIDTH] = {0};
    //Marquage du départ et de l'arrivée sur la map par 1 pour le départ et -2 pour l'arrivée
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
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
        for (int i = 0; i < HEIGHT && !est_trouvee; i++)
        {
            for (int j = 0; j < WIDTH && !est_trouvee; j++)
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
                    if (HEIGHT - 1 > i && map[i + 1][j] == 0)
                    {
                        recherche[i + 1][j] = map[i][j] + 1;
                    }
                    if (j > 0 && map[i][j - 1] == 0)
                    {
                        recherche[i][j - 1] = map[i][j] + 1;
                    }
                    if (WIDTH - 1 > j && map[i][j + 1] == 0)
                    {
                        recherche[i][j + 1] = map[i][j] + 1;
                    }
                }
            }
        }

        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
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

int main()
{

    int map[HEIGHT][WIDTH];
    Coordonnees chemin[HEIGHT * WIDTH] = {{0, 0}};

    maptest(map);
    Coordonnees depart;
    depart.x = 2;
    depart.y = 2;
    Coordonnees arrivee;
    arrivee.x = 22;
    arrivee.y = 22;
    chemins(map, chemin, depart, arrivee);
    for (int i = 0; i < HEIGHT * WIDTH; i++)
    {
        if (chemin[i].x != 0 && chemin[i].y != 0)
        {
            map[chemin[i].y][chemin[i].x] = -3;
        }
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (map[i][j] == -1)
            {
                printf("\033[0;41m");
                printf("  ");
            }
            else if (map[i][j] == 0)
            {
                printf("\033[0;42m");
                printf("  ");
            }
            else if (map[i][j] == -2)
            {
                printf("\033[0;42m");
                printf("AA");
            }
            else if (map[i][j] == -3)
            {
                printf("\033[0;44m");
                printf("  ");
            }
            else if (map[i][j] == 1)
            {
                printf("\033[0;42m");
                printf("DD");
            }
            else if (map[i][j] >= 10)
            {
                printf("\033[0;42m");
                printf("%d", map[i][j]);
            }
            else
            {
                printf("\033[0;42m");
                printf(" %d", map[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
    printf("\033[0;40m");
}
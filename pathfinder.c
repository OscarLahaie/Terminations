#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 40
#define HEIGHT 40

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

void chemins(int map[HEIGHT][WIDTH], Coordonnees depart, Coordonnees arrivee)
{
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
    for (int nombre = 0; nombre < 50; nombre++)
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                if (map[i][j] >= 1)
                {
                    if (i > 0 && map[i - 1][j] == 0)
                    {
                        map[i - 1][j] = map[i][j] + 1;
                    }
                    if (HEIGHT - 1 > i && map[i + 1][j] == 0)
                    {
                        map[i + 1][j] = map[i][j] + 1;
                    }
                    if (j > 0 && map[i][j - 1] == 0)
                    {
                        map[i][j - 1] = map[i][j] + 1;
                    }
                    if (WIDTH - 1 > j && map[i][j + 1] == 0)
                    {
                        map[i][j + 1] = map[i][j] + 1;
                    }
                }
            }
        }
    }
}

int main()
{

    int map[HEIGHT][WIDTH];

    maptest(map);
    Coordonnees depart;
    depart.x = 5;
    depart.y = 5;
    Coordonnees arrivee;
    arrivee.x = 20;
    arrivee.y = 20;
    chemins(map, depart, arrivee);

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
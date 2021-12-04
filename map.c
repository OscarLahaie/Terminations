#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void afficher(int map[50][50], int size) {
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (map[colonne][ligne] < 0) {
                printf("\033[0;44m");
            } else if (map[colonne][ligne] == 0) {
                printf("\033[0;46m");                
            }
            else {
                printf("\033[0;42m");
            }
            printf("  ");
        }
        printf("\033[0m \n");
    }
    printf("\033[0m");
}

int randomize (int map[50][50], int size) {
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 500 < 1)
            {
            map[colonne][ligne] = -1 * (rand() % 7);                
            }
            else {
            map[colonne][ligne] = 1;
            }
        }
    }
    for (int i = 0; i < 10; i++) {
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (map[colonne][ligne] < 0) {
                if (colonne -1 >= 0) {
                map[colonne-1][ligne] = -1 * (rand() % map[colonne][ligne]);
                }
                if (colonne +1 <= 50) {
                map[colonne+1][ligne] = -1 * (rand() % map[colonne][ligne]);
                }
                if (ligne - 1 >= 0) {
                map[colonne][ligne-1] = -1 * (rand() % map[colonne][ligne]);
                }
                if (ligne + 1 <= 50) {
                map[colonne][ligne+1] = -1 * (rand() % map[colonne][ligne]);
                }
            }
        }
    }
    }
    return map;
}

int main () {
    srand(time(NULL));
    int map[50][50] = {0};
    **map = randomize(map, 50);
    afficher(map, 50);
}
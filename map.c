#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void afficher(int map[50][50], int size, int type) {
    if (type == 0) {
        printf("%d\n", map[0][0]);
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en prairie
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                // Couleur du fond
                if (map[colonne][ligne] < 0) {
                    //Bleu foncé pour le fond de l'eau
                    printf("\033[0;44m");
                } else if (map[colonne][ligne] == 0) {
                    // Bleu clair pour les bords
                    printf("\033[0;46m");                
                } else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2){
                    // Vert pour les prairies et les arbres
                    printf("\033[0;42m");
                } else if (map[colonne][ligne] <= 5) {
                    printf("\033[0;43m");
                } else {
                    // Blanc pour les montagnes enneigées 
                    printf("\033[0;47m");
                }
                // Fin couleur du fond
                // Couleur + caractères
                if (map[colonne][ligne] == 2) {
                    printf("\033[2;32m");
                    printf("XX"); 
                } else if (map[colonne][ligne] > 5) {
                    printf("\033[2;30m");
                    printf("MM");
                }
                else {
                    printf("  "); 
                }
                // Fin couleur + caractères
            }
            printf("\033[0m \n");
        }
        printf("\033[0m\n");
    } else if (type == 1) {
        printf("%d\n", map[0][0]);
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en glace
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                // Couleur du fond
                if (map[colonne][ligne] < 0) {
                    //Bleu foncé pour le fond de l'eau
                    printf("\033[0;44m");
                } else if (map[colonne][ligne] == 0) {
                    // Bleu clair pour les bords
                    printf("\033[0;46m");                
                } else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2){
                    // blanc neige 
                    printf("\033[0;47m");
                } 
                // Fin couleur du fond
                // Couleur + caractères
                if (map[colonne][ligne] == 2) {
                    printf("\033[2;30m");
                    printf("XX"); 
                } else if (map[colonne][ligne] <= 0)
                {
                    printf("\033[2;37m");
                    printf("//"); 
                }
                else {
                    printf("  "); 
                }
                // Fin couleur + caractères
            }
            printf("\033[0m \n");
        }
        printf("\033[0m\n");
    }
    else if (type == 2) {
        printf("%d\n", map[0][0]);
        map[0][0] = 1; // Bug à regler le premier element est ici indéfini donc je le fixe le mettant en glace
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                // Couleur du fond
                if (map[colonne][ligne] < 0) {
                    //Bleu clair oasis
                    printf("\033[0;46m");
                } else if (map[colonne][ligne] == 0) {
                    // Vert pour le contour luxuriant
                    printf("\033[0;42m");                
                } else if (map[colonne][ligne] == 1 || map[colonne][ligne] == 2){
                    // désert jaune
                    printf("\033[0;43m");
                } 
                // Fin couleur du fond
                // Couleur + caractères
                if (map[colonne][ligne] == 2) {
                    printf("\033[2;32m");
                    printf("##"); 
                } else if (map[colonne][ligne] == 0)
                {
                    printf("\033[2;32m");
                    printf("XX"); 
                }
                else {
                    printf("  "); 
                }
                // Fin couleur + caractères
            }
            printf("\033[0m \n");
        }
        printf("\033[0m\n");
    }
}


int classique (int map[50][50], int size) {
    // Mise aléatoire de l'eau
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 1000 < 1)
            {
            map[colonne][ligne] = -1 * (rand() % 10);                
            }
            else {
            map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);
    
    for (int i = 0; i < 5; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] < 0) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de la mise en place de l'eau

    // Mise en place des montagnes

    for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (rand() % 500 < 1 && map[colonne][ligne] == 1)
                {
                map[colonne][ligne] = 10 + rand() % 20;                
                }
            }
        }
        
     for (int i = 0; i < 1; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] > 10) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = 10 + (rand() % (map[colonne][ligne]-10));
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = 10 + (rand() % (map[colonne][ligne]-10));
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = 10 + (rand() % (map[colonne][ligne]-10));
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = 10 + (rand() % (map[colonne][ligne]-10));
                    }
                }
            }
        }
    }
    
    // Fin de la mise en place des montagnes


    // Mise en place des arbres
    
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 1000 < 1 && map[colonne][ligne] == 1)
            {
            map[colonne][ligne] = 1 + rand() % 5;                
            }
        }
    }
     for (int i = 0; i < 3; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] > 1  && map[colonne][ligne] < 10) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = 1 + (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    
    //Fin de la mise en place des arbres

    //Harmonisation des reliefs
    for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] < 10 && map[colonne][ligne] >= 2)
                {
                map[colonne][ligne] = 2;                
                }
            }
        }
    for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] >= 10)
                {
                map[colonne][ligne] -= 6;                
                }
            }
        }
    return map;
}

int toundra (int map[50][50], int size) {

    // Mise en place de la glace
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 1000 < 1)
            {
            map[colonne][ligne] = -1 * (rand() % 10);                
            }
            else {
            map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);
    
    for (int i = 0; i < 5; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] < 0) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de la glace

    // Mise en place des arbres 
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 1000 < 1 && map[colonne][ligne] == 1)
            {
            map[colonne][ligne] = 1 + rand() % 5;                
            }
        }
    }
     for (int i = 0; i < 3; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] > 1  && map[colonne][ligne] < 10) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = 1 + (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = 1 + (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin des arbres

}

int desert (int map[50][50], int size) {

    // Mise en place des oasis
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 1000 < 1)
            {
            map[colonne][ligne] = -1 * (rand() % 10);                
            }
            else {
            map[colonne][ligne] = 1;
            }
        }
    }

    //afficher(map, 50, map[0][0]);
    
    for (int i = 0; i < 5; i++) {
        for (int colonne = 0; colonne < size; colonne++){
            for (int ligne = 0; ligne < size; ligne++) {
                if (map[colonne][ligne] < 0) {
                    if (colonne -1 >= 0 && map[colonne-1][ligne] == 1) {
                        map[colonne-1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (colonne +1 <= 50 && map[colonne+1][ligne] == 1) {
                        map[colonne+1][ligne] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne - 1 >= 0 && map[colonne][ligne-1] == 1) {
                        map[colonne][ligne-1] = -1 * (rand() % map[colonne][ligne]);
                    }
                    if (ligne + 1 <= 50 && map[colonne][ligne+1] == 1) {
                        map[colonne][ligne+1] = -1 * (rand() % map[colonne][ligne]);
                    }
                }
            }
        }
    }
    // Fin de oasis

    // Mise en place des cactus
    for (int colonne = 0; colonne < size; colonne++){
        for (int ligne = 0; ligne < size; ligne++) {
            if (rand() % 200 < 1 && map[colonne][ligne] == 1)
            {
            map[colonne][ligne] = 2;                
            }
        }
    }
    
    // Fin des arbres

}

int main () {
    srand(time(NULL));
    int map[50][50] = {0};


    **map = classique(map, 50);
    afficher(map, 50, 0);

    **map = toundra(map, 50);
    afficher(map, 50, 1);
    
    **map = desert(map, 50);
    afficher(map, 50, 2);
}
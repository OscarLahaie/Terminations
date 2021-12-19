#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int globalscore = 0;
int grid[4][4] = {0};
/*void printscore(int score) {
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A");
	}
	globalscore += score;
	int msglen = 9;
	if (globalscore < 0) msglen++;
	int tempscore = globalscore;
	while (tempscore / 10 != 0) {
		msglen++;
		tempscore /= 10;
	}
	printf("\033[41;2mSCORE : %d%*s\033[0m", globalscore, ccount - msglen, "q : quit, h : help");
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
}*/

void clear() {
	for (int i = 0; i < 4; i++) {
		printf("\r\033[2K\033[A");
	}
	for (int i = 0; i < 4; i++) {
		printf("\033[B\r");
	}
}

void printgrid() {
	clear();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d\t\t", grid[i][j]);
		}
		if (i != 3) printf("\n\r");
	}
}

/*void printhelp() {
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A\033[2K");
	}
	printf("This game is about...\n\r\033[2mControls\033[0m : ..., ESC to return to game");
	system("/usr/bin/stty raw");
	getchar();
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
	printscore(0);
}*/

void create_random_piece() {
	int duo[2] = {0};
	int** duos;
	duos = malloc(sizeof(int) * 2 * 0);
	bool found = false;
	int duos_size = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (grid[i][j] == 0) {
				found = true;
				duos_size += 1;
				duos = realloc(duos, duos_size * 2 * sizeof(duo));
				duo[0] = i;
				duo[1] = j;
				duos[duos_size - 1] = duo;
			}
		}
	}
	if (!found) {
		printf("\rGame over, your score : %d\n", globalscore);
		exit(0);
	}
	int randomvalue = 0;
	if (rand() % 10 == 0) randomvalue = 4;
	else randomvalue = 2;
	int random_int = rand() % duos_size;
	int random_x = duos[random_int][0];
	int random_y = duos[random_int][1];
	grid[random_x][random_y] = randomvalue;
	clear();
	printf("\rRand : %d %d\n", random_x, random_y);
	free(duos);
}

void create_random_grid() {
	int random_x = rand() % 4;
	int random_y = rand() % 4;
	grid[random_y][random_x] = 2;
}

void move_pieces(int direction) {
	switch (direction) {
		case 0: //To right
			for (int i = 0; i < 4; i++) {
				int sum_of_line = 0;
				for (int j = 0; j < 4; j++) {
					sum_of_line += grid[i][j];
				}
				if (sum_of_line != 0) {
					for (int n = 0; n < 4; n++) {
						for (int j = 3; j > 0; j--) {
							int sum_of_right = 45;
							while (grid[i][j] == 0 && sum_of_right != 0) {
								sum_of_right = 0;
								for (int k = j; k > 0; k--) {
//									printf("\rgrid[%d][%d] = %d <- grid[%d][%d] = %d\n", i, k, grid[i][k], i, k + 1, grid[i][k - 1]);
									grid[i][k] = grid[i][k - 1];
								}
								for (int k = j; k > 0; k--) {
									sum_of_right += grid[i][k];
								}
//								printgrid();
								grid[i][0] = 0;
							}
						}
						for (int j = 3; j > 0; j--) {
							if (grid[i][j] == grid[i][j - 1] && grid[i][j] != 0) {
								grid[i][j] *= 2;
								grid[i][j - 1] = 0;
							}
						}
					}
				}
			}
			break;
		case 2: //To left
			for (int i = 0; i < 4; i++) {
				int sum_of_line = 0;
				for (int j = 0; j < 4; j++) {
					sum_of_line += grid[i][j];
				}
				if (sum_of_line != 0) {
					for (int n = 0; n < 4; n++) {
						for (int j = 0; j < 3; j++) {
							int sum_of_left = 45;
							while (grid[i][j] == 0 && sum_of_left != 0) {
								sum_of_left = 0;
								for (int k = j; k < 3; k++) {
//									printf("\rgrid[%d][%d] = %d <- grid[%d][%d] = %d\n", i, k, grid[i][k], i, k + 1, grid[i][k + 1]);
									grid[i][k] = grid[i][k + 1];
								}
								for (int k = j; k < 4; k++) {
									sum_of_left += grid[i][k];
								}
//								printgrid();
								grid[i][3] = 0;
							}
						}
						for (int j = 0; j < 3; j++) {
							if (grid[i][j] == grid[i][j + 1] && grid[i][j] != 0) {
								grid[i][j] *= 2;
								grid[i][j + 1] = 0;
							}
						}
					}
				}
			}
			break;
		default:
			break;
	}
}

//void* printinput(void* ptr) {
void* printinput() {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q') {
		switch(c) {
			case 'A':
			case 'B':
			case 'C':
				move_pieces(0);
				break;
			case 'D':
				move_pieces(2);
				break;
			case 'f':
				exit(0);
				break;
			default:
				break;
		}
		create_random_piece();
		printgrid();
	}
	return NULL;
}

int main ()
{
	srand(time(NULL));
	create_random_grid();
	printgrid();
	pthread_t getinput;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_join(getinput, NULL);
}
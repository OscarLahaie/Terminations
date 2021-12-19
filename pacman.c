#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

char plateau[32][30] = {
"||--------------------------||",
"||--------------------------||",
"|| ____ _____ || _____ ____ ||",
"|| |  | |   | || |   | |  | ||",
"|| |  | |   | || |   | |  | ||",
"|| |__| |___| || |___| |__| ||",
"||                          ||",
"|| ____ __ ________ __ ____ ||",
"|| |__| || |__  __| || |__| ||",
"||      ||    ||    ||      ||",
};

char dots[32][30] = {{'X'}};
char persos[32][30] = {{'X'}};
bool isinmenu = false;
int score = 0;
int lives = 3;
int cherries = 0;
int ccount;
int lcount;

void printgrid() {
	fflush(stdout);
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[2K");
		for (int j = 0; j < ccount; j++) {
			if (i >= (lcount - 32) / 2 && i <= (lcount + 32) / 2) {
				if (j >= (ccount - 30) / 2 && j <= (ccount + 30) / 2) {
//					printf("\r%d %d\n", i + 16 - lcount / 2, j + 15 - ccount / 2);
					printf("%c", dots[i + 16 - lcount / 2][j + 15 - ccount / 2]);
				} else
				printf(" ");
			} else
				printf(" ");
		}
		printf("\033[A");
	}
	int scorelen = score;
	int strilen = 1;
	while (scorelen > 10) {
		strilen++;
		scorelen /= 10;
	}
	char* livesstring = malloc(23 * sizeof(char));
	sprintf(livesstring, "Lives : %d, Cherries : %d", lives, cherries);
	char* end = "h : help, q : exit";
	char* titlestring = malloc((18 + strilen) * sizeof(char));
	sprintf(titlestring, "Pacman -- Score : %d", score);
	int a, b;
	a = (ccount - strlen(livesstring)) / 2 - strlen(titlestring);
	b = (ccount - strlen(livesstring)) / 2 - strlen(end);
	printf("\r\033[31;47m%s%*c%s%*c%s\033[0m", titlestring, a, ' ', livesstring, b, ' ', end);
	for (int i = 0; i < ccount; i++) {
		printf("\033[B\r");
	}
}

void* printinput() {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q') {
		switch(c) {
			case 'A':
				break;
			case 'B':
				break;
			case 'C':
				break;
			case 'D':
				break;
			case 'h':
				break;
			default:
				break;
		}
	}
	printgrid();
	return NULL;
}

void* moveghosts() {
	while (true) {
		if (!isinmenu) {
			usleep(500000);
			printf("Move");
		}
	}
}

int main() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row;
	srand(time(NULL));
	printgrid();
	pthread_t getinput;
	pthread_t move_ghosts;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_create(&move_ghosts, NULL, moveghosts, NULL);
	pthread_join(getinput, NULL);
}
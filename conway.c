#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int lcount;
int ccount;
int player_h = 0;
int player_l = 0;
bool** current;
bool** next;
bool firstlaunch = true;
bool computing = false;

void calculate() {
	for (int i = 0; i < lcount; i++) {
		for (int j = 0; j < ccount; j++) {
			int sum_around = 0;
			int lborder = 0;
			int rborder = 0;
			int tborder = 0;
			int bborder = 0;
			if (j == ccount - 1) rborder = 1;
			if (j == 0) lborder = 1;
			if (i == lcount - 1) tborder = 1;
			if (i == 0) bborder = 1;
			for (int k = bborder - 1; k <= 1 - tborder; k++) {
				for (int l = lborder - 1; l <= 1 - rborder; l++) {
					if (current[i + k][j + l] && (k != 0 || l != 0)) {
						sum_around += 1;
					}
				}
			}
//			if (sum_around != 0) printf("\r%d\n", sum_around);
			if (current[i][j]) {
				if (sum_around == 2 || sum_around == 3)
					next[i][j] = true;
				else
					next[i][j] = false;
			} else {
				if (sum_around == 3)
					next[i][j] = true;
				else
					next[i][j] = false;
			}
		}
	}
	for (int i = 0; i < lcount; i++) {
		for (int j = 0; j < ccount; j++) {
			current[i][j] = next[i][j];
		}
	}
}

void clearscreen() {
	printf("\e[1;1H\e[2J");
}

void markcase() {
	current[player_h][player_l] = !current[player_h][player_l];
}


void printgrid() {
	fflush(stdout);
	for (int i = 0; i < lcount; i++) {
		for (int j = 0; j < ccount; j++) {
			if (i == player_h && j == player_l) {
				printf("\033[41m \033[0m");
			} else if (current[i][j]) {
				printf("\033[47m \033[0m");
			} else {
				printf(" ");
			}
		}
		printf("\r\033[A");
	}
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B");
	}
}

void* play(void* ptr) {
	while (true) {
		if (computing) {
			calculate();
			printgrid();
			usleep(100000);
		}
	}
}

void printinput(void) {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q') {
		printf("\r\033[2K");
		switch(c) {
			case 'A':
				if (player_h != lcount - 1) player_h++;
				break;
			case 'B':
				if (player_h != 0) player_h--;
				break;
			case 'C':
				if (player_l != ccount - 1) player_l++;
				break;
			case 'D':
				if (player_l != 0) player_l--;
				break;
            case ' ':
            	if (firstlaunch) {
            		computing = true;
	            	pthread_t player;
    	        	pthread_create(&player, NULL, play, NULL);
    	        	firstlaunch = false;
    	        } else computing = !computing;
				break;
			case 'm':
				markcase();
				break;
			default:
				break;
		}
		printgrid();
	}
	exit(0);
}

int main (int argc, char **argv) {
	clearscreen();
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row;
	current = (bool**) malloc(sizeof(bool*) * lcount);
	next = (bool**) malloc(sizeof(bool*) * lcount);
	for (int i = 0; i < lcount; i++) {
		current[i] = (bool*) malloc(sizeof(bool) * ccount);
		next[i] = (bool*) malloc(sizeof(bool) * ccount);
		for (int j = 0; j < ccount; j++) {
			current[i][j] = false;
			next[i][j] = false;
		}
	}
	printinput();
	system("clear");
	system("reset");
}
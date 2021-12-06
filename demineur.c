#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int timer = 0;
int lcount;
int ccount;
//int* linarr;
//int* colarr;
bool finished = false;
bool isinmenu = false;
bool win = false;
int player_h = 0;
int player_l = 0;
int difficulty = 0;
char** usergrid;
char** minegrid;
int totalmines;

void clearscreen() {
	printf("\e[1;1H\e[2J");
}

void countcases() {
	int total = ccount * lcount;
	int found = 0;
	for (int i = 0; i < lcount; i++) {
		for (int j = 0; j < ccount; j++) {
			if (usergrid[i][j] == 'X')
				found++;
		}
	}
	if (found + totalmines == total) {
		clearscreen();
		printf("\r\033[32mYou won !\033[0m\n");
		exit(0);
	}
}

void markcase() {
	if (usergrid[player_h][player_l] == 'X')
		usergrid[player_h][player_l] = 'M';
	else if (usergrid[player_h][player_l] == 'M')
		usergrid[player_h][player_l] = 'X';
}


int get_color_from_char(char nbr) {
	int color;
	switch(nbr) {
		case 'X': color = 15; break;
		case '0': color = 0; break;
		case '1': color = 4; break;
		case '2': color = 2; break;
		case '3': color = 1; break;
		case '4': color = 20; break;
		case '5': color = 52; break;
		case '6': color = 6; break;
		case '7': color = 5; break;
		case '8': color = 3; break;
		case 'M': color = 90; break;
		case 'O': color = 15; break;
	}
	return color;
}

void showscore() {
//	clearscreen();
	fflush(stdout);
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A");
	}
	int msglen = 9;
	if (timer < 0) msglen++;
	int temptimer = timer;
	while (temptimer / 10 != 0) {
		msglen++;
		temptimer /= 10;
	}
	printf("\033[41;2mTime : %d%*s\033[0m", timer, ccount - msglen, "q : quit, h : help");
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B");
	}
//	printgrid();
}

void printgrid() {
	fflush(stdout);
	showscore();
	for (int i = 0; i < lcount - 1; i++) {
		for (int j = 0; j < ccount; j++) {
			if (usergrid[i][j] == 'O') {
				if (i == player_h && j == player_l) {
					printf("\033[30;41m%c\033[0m", (char) minegrid[i][j]);
				} else {
					printf("\033[38;5;%dm%c\033[0m", get_color_from_char((char) minegrid[i][j]), (char) minegrid[i][j]);
				}
			} else {
				char c;
				if (usergrid[i][j] == 'M') c = '?';
				else c = '+';
				if (i == player_h && j == player_l) {
					printf("\033[30;41m%c\033[0m", c);
				} else {
					printf("\033[38;5;%dm%c\033[0m", get_color_from_char((char) usergrid[i][j]), c);
				}
			}
		}
		printf("\r\033[A");
	}
	for (int i = 0; i < lcount - 1; i++) {
		printf("\r\033[B");
	}
}

void showcase() {
	if (usergrid[player_h][player_l] == 'X') {
		if (minegrid[player_h][player_l] == 'X') {
			clearscreen();
			printf("\r\033[31mYou hit a mine\033[0m\n");
			finished = true;
			exit(0);
		} else if (minegrid[player_h][player_l] == '0') {
			usergrid[player_h][player_l] = 'O';
			if (player_l > 0 && usergrid[player_h][player_l - 1] != 'O') {
				player_l--;
				showcase();
				player_l++;
			}
			if (player_l < ccount - 1 && usergrid[player_h][player_l + 1] != 'O') {
				player_l++;
				showcase();
				player_l--;
			}
			if (player_h > 0 && usergrid[player_h - 1][player_l] != 'O') {
				player_h--;
				showcase();
				player_h++;
			}
			if (player_h < lcount - 1 && usergrid[player_h + 1][player_l] != 'O') {
				player_h++;
				showcase();
				player_h--;
			}
		}
		else
			usergrid[player_h][player_l] = 'O';
	}
	countcases();
}

void printhelp() {
	isinmenu = true;
/*	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A\033[2K");
	}*/
	clearscreen();
	printf("This game is like the classical minesweeper\n\r\033[2mControls\033[0m :\n\rMove Left, Up, Right, Down : \033[31mLeft arrow\033[0m, \033[32mUp arrow\033[0m, \033[33mRight arrow\033[0m, \033[34mDown arrow\033[0m\n\rMark as bomb : \033[35mSpace\033[0m\n\rSelect click case : \033[36mEnter\033[0m\n\rPress any key to return to game");
	system("/usr/bin/stty raw");
	getchar();
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
	isinmenu = false;
}

void* printinput(void*) {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q') {
		printf("\r\033[2K");
//		putchar(c);
//		if (!isinmenu) {
			switch(c) {
				case 'A':
					if (player_h != lcount - 1) player_h++;
					break;
				case 'B':
					if (player_h != 0) player_h--;
					break;
				case 'C':
					if (player_l != ccount) player_l++;
					break;
				case 'D':
					if (player_l != 0) player_l--;
					break;
				case ' ':
					showcase();
					break;
				case 'm':
					markcase();
					break;
				case 'h':
					printhelp();
					break;
				default:
					break;
			}
//			showscore();
			printgrid();
//		}
	}
}

void generategrid() {
	int p_mines = 0;
	switch(difficulty) {
		case 1:
			p_mines = 10;
			break;
		case 2:
			p_mines = 15;
			break;
		case 3:
			p_mines = 20;
			break;
	}
	totalmines = p_mines * (lcount * ccount) / 100;
	for (int i = 0; i < totalmines; i++) {
		int lrandom = random() % lcount;
		int hrandom = random() % ccount;
		while (minegrid[lrandom][hrandom] == 'X' || (lrandom == 0 && hrandom == 0)) {
			lrandom = random() % lcount;
			hrandom = random() % ccount;
		}
		minegrid[lrandom][hrandom] = 'X';
	}
	for (int i = 0; i < lcount; i++) {
		for (int j = 0; j < ccount; j++) {
			if (minegrid[i][j] != 'X') {
				int l_w = 0;
				int r_w = 0;
				int u_w = 0;
				int d_w = 0;
				if (i == 0) d_w = 1;
				if (i == lcount - 1) u_w = 1;
				if (j == 0) l_w = 1;
				if (j == ccount - 1) r_w = 1;
				int mcount_around = 0;
				for (int r = l_w - 1; r <= 1 - r_w; r++) {
					for (int u = d_w - 1; u <= 1 - u_w; u++) {
						if (minegrid[i + u][j + r] == 'X') {
							mcount_around++;
						}
					}
				}
				minegrid[i][j] = mcount_around + '0';
			}
		}
	}
}

void* updatetimer(void*) {
	while (finished == false) {
		sleep(1);
		if (!isinmenu) {
			timer++;
//			showscore();
			printgrid();
		}
	}
}

void createlevel() {
	int diff = 0;
	while (diff > 3 || diff <= 0) {
		printf("\rPlease select a difficulty [1-3]\n> ");
		int c;
		c = getchar();
		int i = c - '0';
		diff = i;
		printf("\e[1;1H\e[2J");
	}
	difficulty = diff;
}

int main (int argc, char **argv) {
	clearscreen();
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row;
	usergrid = (char**) malloc(sizeof(char*) * lcount);
	minegrid = (char**) malloc(sizeof(char*) * lcount);
	for (int i = 0; i < lcount; i++) {
		usergrid[i] = (char*) malloc(sizeof(char) * ccount);
		minegrid[i] = (char*) malloc(sizeof(char) * ccount);
		for (int j = 0; j < ccount; j++) {
			usergrid[i][j] = 'X';
			minegrid[i][j] = 'O';
		}
	}
	createlevel();
	generategrid();
//	showscore();
	printgrid();
	pthread_t getinput;
	pthread_t launchtimer;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_create(&launchtimer, NULL, updatetimer, NULL);
	pthread_join(getinput, NULL);
	system("clear");
	system("reset");
}
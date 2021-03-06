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
bool isinmenu = false;
bool win = false;
int player_h = 0;
int player_l = 0;
int difficulty = 0;
char** usergrid;
char** minegrid;
int totalmines;
bool began = false;

void generategrid(int h, int l) {
	int p_mines = 0;
	switch(difficulty) {
		case 1:
			p_mines = 13;
			break;
		case 2:
			p_mines = 15;
			break;
		case 3:
			p_mines = 20;
			break;
	}
	totalmines = p_mines * ((lcount - 1) * ccount) / 100;
	for (int i = 0; i < totalmines; i++) {
		int lrandom = rand() % (lcount - 1);
		int hrandom = rand() % ccount;
		while (minegrid[lrandom][hrandom] == 'X' || (lrandom == h && hrandom == l)) {
			lrandom = rand() % (lcount - 1);
			hrandom = rand() % ccount;
		}
		minegrid[lrandom][hrandom] = 'X';
	}
	for (int i = 0; i < (lcount - 1); i++) {
		for (int j = 0; j < ccount; j++) {
			if (minegrid[i][j] != 'X') {
				int l_w = 0;
				int r_w = 0;
				int u_w = 0;
				int d_w = 0;
				if (i == 0) d_w = 1;
				if (i == lcount - 2) u_w = 1;
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

void clearscreen() {
	printf("\033[1;1H\033[2J");
}

void countcases() {
	int found = 0;
	for (int i = 0; i < (lcount - 1); i++) {
		for (int j = 0; j < ccount; j++) {
			if (usergrid[i][j] != 'X')
				found++;
		}
	}
	if (found == totalmines) {
		clearscreen();
		printf("\r\033[32mYou won !\033[0m\n");
		system("clear");
		system("reset");
		printf("Time : %d seconds\n", timer);
		printf("\033[?25h");
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
	for (int i = 0; i < (lcount - 1); i++) {
		for (int j = 0; j < ccount; j++) {
			if (usergrid[i][j] == 'O') {
				if (i == player_h && j == player_l) {
					printf("\033[30;41m%c\033[0m", (char) minegrid[i][j]);
				} else {
					printf("\033[38;5;%dm%c\033[0m", get_color_from_char((char) minegrid[i][j]), (char) minegrid[i][j]);
				}
			} else {
				char c;
				if (usergrid[i][j] == 'M') c = '!';
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
	for (int i = 0; i < (lcount - 1); i++) {
		printf("\r\033[B");
	}
}

void showcase() {
	if (!began) {
		generategrid(player_h, player_l);
		began = true;
	}
	if (usergrid[player_h][player_l] == 'X') {
		if (minegrid[player_h][player_l] == 'X') {
			clearscreen();
			system("clear");
			system("reset");
			printf("\r\033[31mYou hit a mine\033[0m\n");
			printf("\033[?25h");
			exit(0);
		} else if (minegrid[player_h][player_l] == '0') {
			usergrid[player_h][player_l] = 'O';
			int l_w = 0;
			int r_w = 0;
			int u_w = 0;
			int d_w = 0;
			if (player_l == 0) d_w = 1;
			if (player_l == lcount - 2) u_w = 1;
			if (player_h == 0) l_w = 1;
			if (player_h == ccount - 1) r_w = 1;
			for (int i = l_w - 1; i <= 1 - r_w; i++) {
				for (int j = d_w - 1; j <= 1 - u_w; j++) {
					if (i != 0 || j != 0) {
						if (usergrid[player_h + i][player_l + j] != 'O') {
							player_h += i;
							player_l += j;
							showcase();
							player_h -= i;
							player_l -= j;
						}
					}
				}
			}
/*			if (player_l > 0 && usergrid[player_h][player_l - 1] != 'O') {
				player_l--;
				showcase();
				player_l++;
			}
			if (player_l < ccount - 2 && usergrid[player_h][player_l + 1] != 'O') {
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
			}*/
		} else
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
	printf("This game is like the classical minesweeper\n\r\033[2mControls\033[0m :\n\rMove Left, Up, Right, Down : \033[31mLeft arrow\033[0m, \033[32mUp arrow\033[0m, \033[33mRight arrow\033[0m, \033[34mDown arrow\033[0m\n\rMark as bomb : \033[35mm\033[0m\n\rClick block : \033[36mSpace\033[0m\n\rPress any key to return to game");
	system("/usr/bin/stty raw");
	getchar();
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
	isinmenu = false;
}

void* printinput() {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q') {
		printf("\r\033[2K");
//		putchar(c);
//		if (!isinmenu) {
			switch(c) {
				case 'A':
					if (player_h != lcount - 2) player_h++;
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
	return NULL;
}

void* updatetimer() {
	while (true) {
		sleep(1);
		if (!isinmenu) {
			timer++;
			showscore();
//			printgrid();
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
		printf("\033[1;1H\033[2J");
	}
	difficulty = diff;
}

int main () {
	srand(time(NULL));
	clearscreen();
	printf("\033[?25l");
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row;
	usergrid = (char**) malloc(sizeof(char*) * (lcount - 1));
	minegrid = (char**) malloc(sizeof(char*) * (lcount - 1));
	for (int i = 0; i < (lcount - 1); i++) {
		usergrid[i] = (char*) malloc(sizeof(char) * ccount);
		minegrid[i] = (char*) malloc(sizeof(char) * ccount);
		for (int j = 0; j < ccount; j++) {
			usergrid[i][j] = 'X';
			minegrid[i][j] = 'O';
		}
	}
	createlevel();
//	generategrid();
//	showscore();
	printgrid();
	pthread_t getinput;
	pthread_t launchtimer;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_create(&launchtimer, NULL, updatetimer, NULL);
	pthread_join(getinput, NULL);
	printf("\033[?25h");
	system("clear");
	system("reset");
}

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int globalscore = 0;
char* b_k = "♔";
char* b_q = "♕";
char* b_r = "♖";
char* b_b = "♗";
char* b_h = "♘";
char* b_p = "♙";
char* w_k = "♚";
char* w_q = "♛";
char* w_r = "♜";
char* w_b = "♝";
char* w_h = "♞";
char* w_p = "♟";

int lcount;
int ccount;

void printhelp() {
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A\033[2K");
	}
	printf("This game is about...\n\r\033[2mControls\033[0m : ..., ESC to return to game");
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 27) {}
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
}

void* printinput(void*) {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 27) {
		printf("\r\033[2K");
		switch(c) {
			case 'a':
			case 's':
			case 'd':
			case 'r':
				break;
			case 'b':
				break;
			case 'h':
				printhelp();
				break;
			default:
				break;
		}
	}
}

int main (int argc, char **argv)
{
    char* grid[8][8] = {
	    {b_r, b_h, b_b, b_q, b_k, b_b, b_h, b_r},
    	{b_p, b_p, b_p, b_p, b_p, b_p, b_p, b_p},
        {" ", " ", " ", " ", " ", " ", " ", " "},
        {" ", " ", " ", " ", " ", " ", " ", " "},
        {" ", " ", " ", " ", " ", " ", " ", " "},
        {" ", " ", " ", " ", " ", " ", " ", " "},
    	{w_p, w_p, w_p, w_p, w_p, w_p, w_p, w_p},
        {w_r, w_h, w_b, w_q, w_k, w_b, w_h, w_r}
	};
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%s", grid[i][j]);
		}
		printf("\n");
	}
	exit(0);

	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    lcount = w.ws_row;
    ccount = w.ws_col;
//	printgrid();
	pthread_t getinput;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_join(getinput, NULL);
}

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int globalscore = 0;
int lcount;
int ccount;
int* linarr;
int* colarr;

void printscore(int score) {
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
	printf("\033[41;2mSCORE : %d%*s\033[0m", globalscore, ccount - msglen, "ESC : quit, h : help");
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
	printf("");
}

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
	printscore(0);
}

void* printinput(void* args) {
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 27) {
		printf("\r\033[2K");
		switch(c) {
			case 'r':
				printscore(30);
				break;
			case 'b':
				printscore(-50);
				break;
			case 'h':
				printhelp();
				break;
			default:
				break;
		}
	}
	return NULL;
}

int main (int argc, char **argv)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row;
	colarr = malloc(sizeof(int) * ccount);
	linarr = malloc(sizeof(int) * ccount * lcount);
	printscore(0);
	pthread_t getinput;
	pthread_create(&getinput, NULL, printinput, NULL);
	pthread_join(getinput, NULL);
}

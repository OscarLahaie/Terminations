#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#define PANEL_LEFT_LEN 30
#define PANEL_BOTTOM_H 5

int lcount;
int ccount;
WINDOW *BOARD[3];
int main() {
	initscr();
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	lcount = w.ws_row;
	ccount = w.ws_col;
	BOARD[0] = newwin(lcount - 10, ccount - 30, 0, 0);
	BOARD[1] = newwin(10, ccount - 30, lcount - 10, 0);
	BOARD[2] = newwin(lcount, 30, 0, ccount - 30);
	for (int i = 0; i < 3; i++) {
        box(BOARD[i], 0, 0);
    }
	mvwprintw(BOARD[0], 0, 1, "Zone principale");
	mvwprintw(BOARD[1], 0, 1, "Options");
	mvwprintw(BOARD[2], 0, 1, "Infos");
	wrefresh(BOARD[0]);
	wrefresh(BOARD[1]);
	wrefresh(BOARD[2]);
}

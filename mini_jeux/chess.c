/**************************************************/
/*              Chess - a chess game              */
/*   Made by SRGotI <fl.valentin1904@gmail.com>   */
/* Source code on https://gitlab.com/srgoti/games */
/**************************************************/

// All the system("stty ...") calls control the way input is entered in the terminal
// `stty raw` gets input from the terminal without the need to press enter
// cbreak() does the same for ncurses input

// Needed for compilation
#define _POSIX_C_SOURCE 200112L
// Needed for network multiplayer
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
// Used to determine size of the terminal window
#include <sys/ioctl.h>
// Used to print text
#include <stdio.h>
// Those are probably needed, not sure tho
#include <unistd.h>
#include <stdlib.h>
// Used to compared strings
#include <string.h>
// Not needed for now
#include <pthread.h>
#include <stdbool.h>
// Used to check if a move or IP address is valid
#include <regex.h>
// Used to display boxes on screen
#include <ncurses.h>
#include <locale.h>

// Define piece names
#define KING 0
#define QUEEN 1
#define ROOK 2
#define BISHOP 3
#define KNIGHT 4
#define PAWN 5

// Define piece colors
#define BLACKS 0
#define WHITES 1

// Define terminal colors
#define BG_BLACK 0
#define BG_RED 1
#define BG_GREEN 2
#define BG_YELLOW 3
#define BG_BLUE 4
#define BG_WHITE 5
#define FG_BLACK 6
#define FG_RED 7
#define FG_GREEN 8
#define FG_YELLOW 9
#define FG_BLUE 10
#define FG_WHITE 11

// Define playing control mode
#define LETTERSMODE 0
#define ARROWSMODE 1

struct Struct {
	bool allowed[2];
	int col[2];
} en_passant;

char* pieces[2][6] = {
	{"♔", "♕", "♖", "♗", "♘", "♙"},
	{"♚", "♛", "♜", "♝", "♞", "♟"},
};
bool has_moved[3] = {false, false, false};
char* lost_pieces[2][16] = {{""}};
int lost_pieces_count[2] = {0};
int lcount;
int ccount;
int mid_h;
int mid_w;
char* grid[8][8] = {{""}};
char* grid_checkmate[8][8] = {{""}};
int sockfd = 0;
int new_fd = 0;
int gamemode = -1;
int cursor_pos_x = 0;
int cursor_pos_y = 0;
int keyboardmode = ARROWSMODE;
bool iswhiteplayer = true;
int servertoclient = 0;
int firstmove = 0;
bool skipturn = 0;
int firstpart = 0;
bool sendpacket = false;
int currentpiece[2] = {8, 8};
int nextplayer = WHITES;
bool promotion = false;
int promotedpiece = 0;
WINDOW* game;
WINDOW* lost_top;
WINDOW* lost_bottom;
WINDOW* input;
WINDOW* hist;
WINDOW* status;
int time_passed = 0;
short COLOR_ORANGE;
// This will be useful to print the inverted grid for the black pieces player
short invertedbuffer[8][8] = {{0}};
char** history;
bool isinmenu = false;

void* timer() {
	while (true) {
		sleep(1);
		time_passed += 1;
		if (!isinmenu) {
			wmove(status, 1, 1);
			wprintw(status, "Chess v1, Time : %d, h : help", time_passed);
			wrefresh(status);
			wmove(input, 1, 1);
		}
	}
}

// Check if a piece is of a certain color
bool belongs(char* piece, int color) {
	for (int i = 0; i < 6; i++) {
		if (strcmp(piece, pieces[color][i]) == 0)
			return true;
	}
	return false;
}

// Circular definition of functions
bool is_move_allowed(int color, int start_y, int start_x, int finish_y, int finish_x);

bool is_case_check(int mycolor, int y, int x) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (belongs(grid[i][j], 1 - mycolor) && is_move_allowed(1 - mycolor, i, j, y, x)) {
				return true;
			}
		}
	}
	return false;
}

// Returns true if a piece can move from (start_y, start_x) to (finish_y, finish_x)
bool is_move_allowed(int color, int start_y, int start_x, int finish_y, int finish_x) {
	// Would be better w/ a switch case but not available as strings are pointers
	if (strcmp(grid[start_y][start_x], pieces[color][KING]) == 0) { // Allowed moves for the king
		if (abs(start_x - finish_x) <= 1 && abs(start_y - finish_y) <= 1) { // Move must be <= 1 cases
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if destination is not a same color piece
				return false;
			} else {
				return true;
			}
		} else if (strcmp(grid[finish_y][finish_x], pieces[color][ROOK]) == 0 && finish_y == (iswhiteplayer ? 0 : 8) && start_y == finish_y) { // Castling
			bool can_do_castling = true;
			switch(finish_x) {
				case 0:
					if (has_moved[0] || has_moved[1]) {
						can_do_castling = false;
					}
					for (int i = start_x; i > finish_x; i--) {
						if (strcmp(grid[start_y][i], " ") != 0 && i != start_x) {
							can_do_castling = false;
						}
						if (is_case_check(color, start_y, i)) {
							can_do_castling = false;
						}
					}
					return can_do_castling;
					break;
				case 7:
					if (has_moved[1] || has_moved[2]) {
						can_do_castling = false;
					}
					for (int i = start_x; i < finish_x; i++) {
						if (strcmp(grid[start_y][i], " ") != 0 && i != start_x) {
							can_do_castling = false;
						}
						if (is_case_check(color, start_y, i)) {
							can_do_castling = false;
						}
					}
					return can_do_castling;
					break;
				default:
					return false;
			}
		} else {
			return false;
		}
	} else if (strcmp(grid[start_y][start_x], pieces[color][QUEEN]) == 0) { // Allowed moves for the king
		if (abs(finish_y - start_y) == 0 || abs(finish_x - start_x) == 0 || abs(finish_x - start_x) == abs(finish_y - start_y)) { // Movement should be vertical, horizontal or diagonal (in this case, queen moves n cases on both axis)
			// Checking movement directions
			int x_delta = 0; // Not moving horizontally
			int y_delta = 0; // Not moving vertically
			if (finish_y - start_y < 0)
				y_delta = -1; // Moving backwards
			else if (finish_y - start_y > 0)
				y_delta = 1; // Moving forwards
			if (finish_x - start_x < 0)
				x_delta = -1; // Moving to the left
			else if (finish_x - start_x > 0)
				x_delta = 1; // Moving to the right
			for (int i = 1; i < ((abs(finish_y - start_y) == 0) ? abs(finish_x - start_x) : abs(finish_y - start_y)); i++) {
				if (strcmp(grid[start_y + y_delta * i][start_x + x_delta * i], " ") != 0) { // Check if any non-empty case is on the path of the queen
					return false;
				}
			}
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if destination is not a same color piece
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else if (strcmp(grid[start_y][start_x], pieces[color][ROOK]) == 0) { // Allowed moves for the rook
		if (abs(finish_y - start_y) == 0 || abs(finish_x - start_x) == 0) { // Rook must move horizontally or vertically
			// Checking movement directions
			int x_delta = 0; // Not moving horizontally
			int y_delta = 0; // Not moving vertically
			if (finish_y - start_y < 0)
				y_delta = -1; // Moving backwards
			else if (finish_y - start_y > 0)
				y_delta = 1; // Moving forwards
			if (finish_x - start_x < 0)
				x_delta = -1; // Moving to the left
			else if (finish_x - start_x > 0)
				x_delta = 1; // Moving to the right
			for (int i = 1; i < (abs(finish_x - start_x) > abs(finish_y - start_y) ? abs(finish_x - start_x) : abs(finish_y - start_y)); i++) {
				if (strcmp(grid[start_y + y_delta * i][start_x + x_delta * i], " ") != 0) { // Check if any non-empty case is on the path of the rook
					return false;
				}
			}
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if destination is not a same color piece
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else if (strcmp(grid[start_y][start_x], pieces[color][BISHOP]) == 0) { // Allowed moves for the bishop
		if (abs(finish_y - start_y) == abs(finish_x - start_x)) { // Rook must move diagonally (bishop moves n cases on both axis)
			// Checking movement directions
			int x_delta = 0; // Not moving horizontally
			int y_delta = 0; // Not moving vertically
			if (finish_y - start_y < 0)
				y_delta = -1; // Moving backwards
			else if (finish_y - start_y > 0)
				y_delta = 1; // Moving forwards
			if (finish_x - start_x < 0)
				x_delta = -1; // Moving to the left
			else if (finish_x - start_x > 0)
				x_delta = 1; // Moving to the right
			for (int i = 1; i < abs(finish_y - start_y); i++) {
				if (strcmp(grid[start_y + y_delta * i][start_x + x_delta * i], " ") != 0) { // Check if any non-empty case is on the path of the bishop
					return false;
				}
			}
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if the destination is not a same color piece
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else if (strcmp(grid[start_y][start_x], pieces[color][KNIGHT]) == 0) { // Allowed moves for the knight
		if ((abs(finish_y - start_y) == 2 && abs(finish_x - start_x) == 1) || (abs(finish_y - start_y) == 1 && abs(finish_x - start_x) == 2)) { // Knight moves 'two squares vertically and one square horizontally, or two squares horizontally and one square vertically' - Wikipedia
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if the destination is not a same color piece
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else if (strcmp(grid[start_y][start_x], pieces[color][PAWN]) == 0 ) { // Allowed moves for the pawn
		int check = iswhiteplayer ? 1 : 6;
		if (color == BLACKS) { // This matters because pawns can only go forward
			if (finish_x == start_x && finish_y - start_y == - 2 && start_y == check) { // Allow two cases in front it pawn is at its start point
				if (strcmp(grid[finish_y][finish_x], " ") == 0) {
					if (strcmp(grid[finish_y + 1][finish_x], " ") != 0) {
						return false;
					} else {
						return true;
					}
				} else {
					return false;
				}
			} else if (finish_y - start_y == - 1 && finish_x == start_x) { // Allow one move in front if nothing is in front of him
				if (strcmp(grid[finish_y][finish_x], " ") == 0) {
					return true;
				} else {
					return false;
				}
			} else if (finish_y - start_y == - 1 && abs(finish_x - start_x) == 1) { // Allow eating diagonally in front if there is an opposite team member
				if (belongs(grid[finish_y][finish_x], 1 - color)) {
					return true;
				} else if (en_passant.allowed[1 - color] && en_passant.col[1 - color] == finish_x && finish_y == 2) {
					return true;
					// En passant
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else {
			if (finish_x == start_x && finish_y - start_y == 2 && start_y == check) { // Allow two cases in front it pawn is at its start point
				if (strcmp(grid[finish_y][finish_x], " ") == 0) {
					if (strcmp(grid[finish_y - 1][finish_x], " ") != 0) {
						return false;
					} else {
						return true;
					}
				} else {
					return false;
				}
			} else if (finish_y - start_y == 1 && finish_x == start_x) { // Allow one move in front if nothing is in front of him
				if (strcmp(grid[finish_y][finish_x], " ") == 0) {
					return true;
				} else {
					return false;
				}
			} else if (finish_y - start_y == 1 && abs(finish_x - start_x) == 1) { // Allow eating diagonally in front if there is an opposite team member
				if (belongs(grid[finish_y][finish_x], 1 - color)) {
					return true;
				} else if (en_passant.allowed[1 - color] && en_passant.col[1 - color] == finish_x && finish_y == 5) {
					return true;
					// En passant
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
	} else { // Empty case selected
		return false;
	}
}

// Is my king in danger RN ?
// This super unoptimized function shows it
bool king_chess(int color) {
	int my_king_y;
	int my_king_x;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (strcmp(pieces[color][KING], grid[i][j]) == 0) {
				my_king_y = i;
				my_king_x = j;
			}
		}
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (is_move_allowed(1 - color, i, j, my_king_y, my_king_x)) {
				return true;
			}
		}
	}
	return false;
}

// User called exit, cleanup
void doexit(int err) {
	//clearscreen();
	erase();
	system("reset");
	if (sockfd != 0)
		close(sockfd);
	if (new_fd != 0)
		close(new_fd);
	exit(err);
}

// Display the game grid on screen
// The grid should display the current player's pieces at the bottom
// Hence the conditions and (1)
void printgrid() {
	printf("\r");
	if (gamemode != 2) {
		werase(status);
		box(status, 0, 0);
		mvwprintw(status, 0, 1, "Status");
		if (king_chess(iswhiteplayer ? WHITES : BLACKS)) {
			wmove(status, 2, 1);
			wattron(status, COLOR_PAIR(FG_RED));
			wprintw(status, "Your king is in danger");
			wattroff(status, COLOR_PAIR(FG_RED));
		}
		wrefresh(status);
	}
	werase(lost_top);
	box(lost_top, 0, 0);
	mvwprintw(lost_top, 0, 1, "Lost pieces");
	wmove(lost_top, 1, 1);
	for (int i = 0; i < lost_pieces_count[iswhiteplayer ? WHITES : BLACKS]; i++) {
		wprintw(lost_top, "%s", lost_pieces[iswhiteplayer ? WHITES : BLACKS][i]);
		if (i == 7)
			wmove(lost_top, 2, 1);
	}
	wrefresh(lost_top);
	wmove(game, 1, 2);
	wprintw(game, "ABCDEFGH");
	for (int i = 0; i < 8; i++) {
		if (!iswhiteplayer) {
			wmove(game, 2 + i, 1);
			wprintw(game, "%d", i + 1);
		}
		for (int j = 0; j < 8; j++) {
			int casecolor;
			// Use color for allowed moves
			if (i == cursor_pos_y && j == cursor_pos_x) {
				if (currentpiece[0] == 8 && currentpiece[1] == 8) { // Player is choosing a piece to move
					if (belongs(grid[i][j], iswhiteplayer ? WHITES : BLACKS)) {
						int movesfound = 0;
						for (int k = 0; k < 8; k++) {
							for (int l = 0; l < 8; l++) {
								if (is_move_allowed(iswhiteplayer ? WHITES : BLACKS, i, j, k, l)) {
									movesfound++;
								}
							}
						}
						if (movesfound != 0) {
							casecolor = BG_GREEN;
						} else {
							casecolor = BG_RED;
						}
					} else {
						casecolor = BG_RED;
					}
				} else {
					if (i == currentpiece[0] && j == currentpiece[1]) {
						casecolor = BG_YELLOW;
					} else if (is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], i, j)) { // Player is choosing a destination to its piece
						casecolor = BG_BLUE;
					} else {
						casecolor = BG_RED;
					}
				}
			} else {
				if (i == currentpiece[0] && j == currentpiece[1]) {
					casecolor = BG_YELLOW;
				} else {
					if (currentpiece[0] != 8 && currentpiece[1] != 8 && is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], i, j)) { // Show all available moves for a piece
						casecolor = BG_GREEN;
					}
					if (strcmp(grid[i][j], " ") == 0) {
						if (currentpiece[0] != 8 && currentpiece[1] != 8 && is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], i, j)) { // Show all available moves for a piece
							casecolor = BG_GREEN;
						} else if ((i + j) % 2 == 0) { // This square is empty, depending on the grid, it will be displayed as black or white
							casecolor = BG_BLACK;
						} else {
							casecolor = BG_WHITE;
						}
					} else {
						if (currentpiece[0] != 8 && currentpiece[1] != 8 && is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], i, j)) { // Show all available moves for a piece
							casecolor = BG_GREEN;
						} else {
							casecolor = BG_BLACK;
						}
					}
				}
			}
// Beginning of (1)
			invertedbuffer[i][j] = casecolor;
			if (!iswhiteplayer) {
				wmove(game, 2 + i, 2 + j);
				wattron(game, COLOR_PAIR(casecolor));
				wprintw(game, grid[i][j]);
				wattroff(game, COLOR_PAIR(casecolor));
			}
		}
		if (!iswhiteplayer) {
			wmove(game, 2 + i, 10);
			wprintw(game, "%d", i + 1);
		}
	}
	if (iswhiteplayer) {
		for (int i = 7; i >= 0; i--) {
			wmove(game, 2 + i, 1);
			wprintw(game, "%d", 8 - i);
			for (int j = 0; j < 8; j++) {
				wmove(game, 9 - i, 2 + j);
				wattron(game, COLOR_PAIR(invertedbuffer[i][j]));
				wprintw(game, grid[i][j]);
				wattroff(game, COLOR_PAIR(invertedbuffer[i][j]));
			}
			wmove(game, 2 + i, 10);
			wprintw(game, "%d", 8 - i);
		}
	}
	wmove(game, 10, 2);
	wprintw(game, "ABCDEFGH");
	wrefresh(game);
// End of (1)

	werase(lost_bottom);
	box(lost_bottom, 0, 0);
	mvwprintw(lost_bottom, 0, 1, "Won pieces");
	wmove(lost_bottom, 1, 1);
	for (int i = 0; i < lost_pieces_count[iswhiteplayer ? BLACKS : WHITES]; i++) {
		wprintw(lost_bottom, "%s", lost_pieces[iswhiteplayer ? BLACKS : WHITES][i]);
		if (i == 7)
			wmove(lost_bottom, 2, 1);
	}
	wrefresh(lost_bottom);
}

// Client connect code
// Most of the code is from a document at http://beej.us/guide/bgnet/
void startconnect() {
	regex_t regex;
	regcomp(&regex, "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$", 0); // Regex for an IP Address, stolen on S.O.
//	clearscreen();
	erase();
	mvprintw(0, 0, "Enter an IP address, then press ");
	attron(COLOR_PAIR(FG_BLUE));
	printw("enter");
	attroff(COLOR_PAIR(FG_BLUE));
	printw(" to validate");
	refresh();
	move(1, 0);
	char* address = malloc(16 * sizeof(char));
	echo();
	cbreak();
	scanw("%s", address);
	noecho();
	while (!regexec(&regex, address, 0, NULL, 0)) {
		erase();
		mvprintw(0, 0, "Enter an IP address, then press ");
		attron(COLOR_PAIR(FG_BLUE));
		printw("enter");
		attroff(COLOR_PAIR(FG_BLUE));
		printw(" to validate");
		move(1, 0);
	}
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(address, "4050", &hints, &res); // Open a connection at address:4050
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	erase();
	mvprintw(0, 0, "Trying to connect");
	struct timeval timeout;
	timeout.tv_sec  = 5;  // Timeout after 5 seconds
	timeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		erase();
		attron(COLOR_PAIR(FG_RED));
		mvprintw(0, 0, "Failed to connect to host");
		attroff(COLOR_PAIR(FG_RED));
		sleep(2);
		doexit(1);
	}
	erase();
	attron(COLOR_PAIR(FG_GREEN));
	mvprintw(0, 0, "Connected");
	attroff(COLOR_PAIR(FG_GREEN));
	mvprintw(1, 0, "Waiting for server to choose color");
	// Server chooses a color, returns the other to client
	int color2;
	recv(sockfd, &color2, sizeof(int), 0);
	erase();
//	clearscreen();
	iswhiteplayer = color2 == WHITES;
}

// Server host code
// Cf same guide
void startserver() {
	struct addrinfo hints, *res;
	struct sockaddr_in client;
	struct sockaddr_in server;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, "4050", &hints, &res);
	new_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	bind(new_fd, res->ai_addr, res->ai_addrlen);
	listen(new_fd, 1);
	erase();
	socklen_t addr_size = sizeof(server);
	memset(&server, 0, sizeof(server));

	mvprintw(0, 0, "Waiting for someone to connect, IP addresses of this pc");
	move(1, 0);
	system("if command -v ip >/dev/null 2>&1; then list=$(for line in $(ip route show); do echo $line; done | grep -A 2 src | grep 192.168) && for elem in $list; do echo -e \r\033[32m$elem\033[0m; done; else echo -e \033[31mcommand `ip` not installed\033[0m; fi");
	sockfd = accept(new_fd, (struct sockaddr *) &client, &addr_size);
	//clearscreen();
	erase();
	struct hostent *hostName;
	struct in_addr ipv4addr;
	inet_pton(AF_INET, inet_ntoa(client.sin_addr), &ipv4addr);
	hostName = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	mvprintw(0, 0, "Received connection from ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("%s", hostName->h_name);
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" with IP address ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("%s", inet_ntoa(client.sin_addr));
	attroff(COLOR_PAIR(FG_GREEN));
//	printf("\rReceived connection from \"%s\", IP address : %s\n", hostName->h_name, inet_ntoa(client.sin_addr));
	erase();
	// Server chooses a color
	mvprintw(0, 0, "What color would you like to play ? Use ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("marrow keys");
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" to move and press ");
	attron(COLOR_PAIR(FG_BLUE));
	printw("menter");
	attroff(COLOR_PAIR(FG_BLUE));
	printw(" to validate");
	char* c1 = "Play as white";
	char* c2 = "Play as black";
	char* list[2] = {c1, c2};
	int color = 0;
	int player_pos = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
//			bgcolor = 30;
			color = FG_RED;
		} else {
			color = FG_WHITE;
		}
		attron(COLOR_PAIR(color));
		mvprintw(i + 1, 0, list[i]);
		attroff(COLOR_PAIR(color));
//		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
//	printf("\r");
//	system("/usr/bin/stty raw");
	int c;
	bool done = false;
	cbreak();
//	raw();
	keypad(stdscr, TRUE);
	while (!done) {
		c = getch();
		printf("\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case KEY_DOWN:
			case 'B':
				if (player_pos != 1)
					player_pos++;
				break;
			case KEY_UP:
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			case KEY_ENTER:
			case '\n':
				done = true;
				break;
			default:
				break;
		}
//		printf("\033[2K");
		for (int i = 0; i < 2; i++) {
			if (player_pos == i) {
				color = FG_RED;
			} else {
				color = FG_WHITE;
			}
			attron(COLOR_PAIR(color));
			mvprintw(i + 1, 0, list[i]);
			attroff(COLOR_PAIR(color));
//			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
//		printf("\r");
	}
//	clearscreen();
	erase();
//	system("/usr/bin/stty cooked");
	int sendblack2;
	switch(player_pos) {
		case 0:
			sendblack2 = 0;
			break;
		case 1:
			sendblack2 = 1;
			break;
		default:
			break;
	}
	// Send to client the other color
	send(sockfd, &sendblack2, sizeof(int), 0);
//	clearscreen();
}

// We are playing in LAN mode, user has to choose to be server or client (doesn't change much in game)
void startaskserver() {
//	clearscreen();
	erase();
	int player_pos = 0;
	mvprintw(0, 0, "Who will be hosting the game ? Use ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("arrow keys");
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" to move and press ");
	attron(COLOR_PAIR(FG_BLUE));
	printw("enter");
	attroff(COLOR_PAIR(FG_BLUE));
	printw(" to validate");
	char* c1 = "Host the game";
	char* c2 = "Connect to a host";
	char* list[2] = {c1, c2};
	int color = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
			color = FG_RED;
		} else {
			color = FG_WHITE;
		}
		attron(COLOR_PAIR(color));
		mvprintw(i + 1, 0, list[i]);
		attroff(COLOR_PAIR(color));
//		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
//	printf("\r");
//	system("/usr/bin/stty raw");
	int c;
	bool done = false;
	cbreak();
//	raw();
	keypad(stdscr, TRUE);
	while (!done) {
		c = getch();
		printf("\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case KEY_DOWN:
			case 'B':
				if (player_pos != 1)
					player_pos++;
				break;
			case KEY_UP:
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			case KEY_ENTER:
			case '\n':
				done = true;
				break;
			default:
				break;
		}
//		printf("\033[2K");
		for (int i = 0; i < 2; i++) {
			if (player_pos == i) {
				color = FG_RED;
			} else {
				color = FG_WHITE;
			}
			attron(COLOR_PAIR(color));
			mvprintw(i + 1, 0, list[i]);
			attroff(COLOR_PAIR(color));
//			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
//		printf("\r");
	}
//	system("/usr/bin/stty cooked");
	switch(player_pos) {
		case 1:
			startconnect();
			break;
		case 0:
			startserver();
			break;
		default:
			break;
	}
}


// Player asked for help
void printhelp() {
	isinmenu = true;
	erase();
	printw("\rThis game is a pretty normal chess game\n");
	printw("\rYou can move your pieces with two different methods :\n");
	printw("\r - \033[32mCursor mode :\033[0m\n");
	printw("\r   Move the cursor with the \033[34marrows\033[0m of your keyboard\n");
	printw("\r   Select a piece and validate its move with \033[33mspacebar\033[0m\n");
	printw("\r   You can cancel a move by selecting the same piece you selected before");
	printw("\r - \033[32mDirect mode :\033[0m\n");
	printw("\r   Note the coordinates of the piece you want to move and where to move it\n");
	printw("\r   Write them with the format [Initial column][Initial line][Final column][Final line] without the brackets\n");
	printw("\r   Validate your input with \033[33menter\033[0m\n");
	printw("\rMeta : this game supports\n");
	printw("\r - The castling :\n");
	printw("\r   You can select the king to move to the rook\n");
	printw("\r - En passant\n");
	printw("\rPress \033[33mESC\033[0m to exit this help\n");
	refresh();
	int c;
	while ((c = getch()) != 27) {}
	erase();
	refresh();
	box(status, 0, 0);
	box(input, 0, 0);
	box(game, 0, 0);
	box(lost_top, 0, 0);
	box(lost_bottom, 0, 0);
	mvwprintw(status, 0, 1, "Status");
	mvwprintw(game, 0, 1, "Grid");
	mvwprintw(input, 0, 1, "Input");
	mvwprintw(lost_bottom, 0, 1, "Won pieces");
	mvwprintw(lost_top, 0, 1, "Lost pieces");
	box(hist, 0, 0);
	mvwprintw(hist, 0, 1, "History");
	for (int i = 0; i < lcount - 6; i++) {
		if (strcmp(history[i], "    ") != 0) {
			mvwprintw(hist, 1 + i, 1, "%c", history[i][0]);
			mvwprintw(hist, 1 + i, 2, "%d", 9 - (int) (history[i][1] - '0'));
			mvwprintw(hist, 1 + i, 3, "%c", history[i][2]);
			mvwprintw(hist, 1 + i, 4, "%d", 9 - (int) (history[i][3] - '0'));
		}
	}
	wrefresh(hist);
	isinmenu = false;
}


// Wait for any playing event
void waitforevent() {
	char* usermove = malloc(4 * sizeof(char));
	while (true) {
		werase(input);
		box(input, 0, 0);
		mvwprintw(input, 0, 1, "Input");
		wmove(input, 1, 1);
		wrefresh(input);
		promotion = false;
		int mycolor = iswhiteplayer ? WHITES : BLACKS;
		en_passant.allowed[mycolor] = false;
		en_passant.col[mycolor] = 8;
		skipturn = false;
		if (sockfd != 0 && !iswhiteplayer && firstmove == 0) {
			// If we are not white player and in lan, wait for the other player to start
			wprintw(input, "Waiting for other player");
			wrefresh(input);
			char* otherguymove = malloc(4 * sizeof(char));
			recv(sockfd, otherguymove, 4, 0);
			if (strcmp(otherguymove, "QUIT") == 0) { // The other player quit, exit now
				//clearscreen();
				erase();
				mvprintw(0, 0, "\rOther player has left\n");
				sleep(2);
				doexit(0);
			}
			sendpacket = false;
			int color = iswhiteplayer ? WHITES : BLACKS;
			int prev[2] = {8 - (otherguymove[1] - '0'), (int) (otherguymove[0]) - 65};
			int next[2] = {8 - (otherguymove[3] - '0'), (int) (otherguymove[2]) - 65};
			if (belongs(grid[next[0]][next[1]], color)) {
				lost_pieces[color][lost_pieces_count[color]] = grid[next[0]][next[1]];
				lost_pieces_count[color] += 1;
			}
			grid[next[0]][next[1]] = malloc(strlen(grid[prev[0]][prev[1]]));
			strcpy(grid[next[0]][next[1]], grid[prev[0]][prev[1]]);
			grid[prev[0]][prev[1]] = malloc(strlen(" "));
			strcpy(grid[prev[0]][prev[1]], " ");
			firstmove = 1;
			// Display the history on the right
			int last_available = -1;
			for (int i = 0; i < lcount - 6; i++) {
				// Get if the history array has an empty line
				if (strcmp(history[i], "    ") == 0) {
					last_available = i;
					break;
				}
			}
			if (last_available == -1) { // The array is full
				for (int i = 1; i < lcount - 6; i++) { // Move every line of the array backwards
					history[i - 1] = malloc(4 * sizeof(char));
					for (int j =  0; j < 4; j++) {
						history[i - 1][j] = ' ';
						history[i - 1][j] = history[i][j];
					}
				}
				history[lcount - 7] = malloc(4 * sizeof(char));
				// Append the last move to the end of the array
				for (int i = 0; i < 4; i++) {
					history[lcount - 7][i] = ' ';
					history[lcount - 7][i] = otherguymove[i];
				}
			} else { // Just add the last move at the first free spot
				history[last_available] = malloc(4 * sizeof(char));
				for (int i = 0; i < 4; i++) {
					history[last_available][i] = ' ';
					history[last_available][i] = otherguymove[i];
				}
			}
			werase(hist);
			box(hist, 0, 0);
			mvwprintw(hist, 0, 1, "History");
			for (int i = 0; i < lcount - 6; i++) {
				if (strcmp(history[i], "    ") != 0) {
					mvwprintw(hist, 1 + i, 1, "%c", history[i][0]);
					mvwprintw(hist, 1 + i, 2, "%d", 9 - (int) (history[i][1] - '0'));
					mvwprintw(hist, 1 + i, 3, "%c", history[i][2]);
					mvwprintw(hist, 1 + i, 4, "%d", 9 - (int) (history[i][3] - '0'));
				}
			}
			wrefresh(hist);
			free(otherguymove);
		} else {
			mvwprintw(input, 1, 1, "Your turn to play");
			wrefresh(input);
			if (keyboardmode == LETTERSMODE) { // In this mode, player moves by entering grid values, e.g : B1C3 on the first turn will move the white left knight to the C3 case
				int prev[2] = {8, 8};
				int next[2] = {8, 8};
				system("/usr/bin/stty cooked");
				while (strcmp(usermove, "WAIT") != 0 && ((prev[0] == 8 && prev[1] == 8 && next[0] == 8 && next[1] == 8) || !is_move_allowed(iswhiteplayer ? WHITES : BLACKS, prev[0], prev[1], next[0], next[1]))) {
					if (prev[0] != 8 && prev[1] != 8 && next[0] != 8 && next[1] != 8) {
						werase(input);
						box(input, 0, 0);
						mvwprintw(input, 0, 1, "Input");
						wattron(input, COLOR_PAIR(FG_RED));
						mvwprintw(input, 2, 1, "Invalid move");
						wattroff(input, COLOR_PAIR(FG_RED));
						wrefresh(input);
						sleep(2);
						mvwprintw(input, 1, 1, "Your turn to play");
					}
					mvwprintw(input, 2, 1, "Enter input (B1C3 style) > ");
					wrefresh(input);
					scanf("%s", usermove);
					if (strcmp(usermove, "h") == 0) {
						usermove = "WAIT";
						printhelp();
					} else if (strcmp(usermove, "q") == 0) { // Player quits, send the info to the other player
						usermove = "QUIT";
						if (sockfd != 0)
							send(sockfd, usermove, 4, 0);
						doexit(0);
					} else {
						regex_t regex;
						regcomp(&regex, "[A-H][1-8][A-H][1-8]", 0);
						while (regexec(&regex, usermove, 0, NULL, 0) != 0) { // Check if the input is valid according to regex
							werase(input);
							box(input, 0, 0);
							mvwprintw(input, 0, 1, "Input");
							wattron(input, COLOR_PAIR(FG_RED));
							mvwprintw(input, 2, 1, "Invalid move");
							wattroff(input, COLOR_PAIR(FG_RED));
							wrefresh(input);
							sleep(2);
							werase(input);
							box(input, 0, 0);
							mvwprintw(input, 0, 1, "Input");
							mvwprintw(input, 2, 1, "Enter input (B1C3 style) > ");
							mvwprintw(input, 1, 1, "Your turn to play");
							wrefresh(input);
							scanf("%s", usermove);
							if (strcmp(usermove, "h") == 0) {
								usermove = "WAIT";
								printhelp();
							} else if (strcmp(usermove, "q") == 0) { // Player quits, send the info to the other player
								usermove = "QUIT";
								doexit(0);
							}
						}
						if (strcmp(usermove, "WAIT") != 0) {
							prev[0] = (usermove[1] - '0') - 1;
							prev[1] = (int) (usermove[0]) - 65;
							next[0] = (usermove[3] - '0') - 1;
							next[1] = (int) (usermove[2]) - 65;
							if (strcmp(grid[prev[0]][prev[1]], pieces[iswhiteplayer ? WHITES : BLACKS][PAWN]) == 0 && abs(prev[0] - next[0]) == 2) { // En passant
								en_passant.allowed[iswhiteplayer ? WHITES : BLACKS] = true;
								en_passant.col[iswhiteplayer ? WHITES : BLACKS] = prev[1];
							}
							if (prev[0] == iswhiteplayer ? 0 : 1) {
								switch(prev[1]) {
									case 0:
										has_moved[0] = true;
										break;
									case 4:
										has_moved[1] = true;
										break;
									case 7:
										has_moved[2] = true;
										break;
									default:
										break;
								}
							}
							usermove[1] = (char) (8 - prev[0] + 48);
							usermove[3] = (char) (8 - next[0] + 48);
						}
					}
				}
				if (strcmp(usermove, "WAIT") != 0) {
					sendpacket = true;
					int color = iswhiteplayer ? BLACKS : WHITES;
					if (belongs(grid[next[0]][next[1]], color)) {
						if (strcmp(grid[next[0]][next[1]], pieces[color][KING]) == 0) {
							//clearscreen();
							erase();
							attron(COLOR_PAIR(FG_GREEN));
							mvprintw(0, 0, "You win");
							attroff(COLOR_PAIR(FG_GREEN));
							char win[4] = "LOST";
							send(sockfd, win, 4, 0);
							sleep(2);
							doexit(0);
						}
						lost_pieces[color][lost_pieces_count[color]] = grid[next[0]][next[1]];
						lost_pieces_count[color] += 1;
					}
					if (strcmp(grid[prev[0]][prev[1]], pieces[1 - color][PAWN]) == 0 && prev[1] != next[1] && strcmp(grid[next[0]][next[1]], " ") == 0) {
						// En passant
						lost_pieces[color][lost_pieces_count[color]] = grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]];
						grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]] = malloc(strlen(" "));
						strcpy(grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]], " ");
						lost_pieces_count[color] += 1;
					}
					if (strcmp(grid[prev[0]][prev[1]], pieces[iswhiteplayer ? WHITES : BLACKS][KING]) == 0 && strcmp(grid[next[0]][next[1]], pieces[iswhiteplayer ? WHITES : BLACKS][ROOK]) == 0) {
						// Castling
						switch(next[1]) {
							case 0:
								// Moving left
								// King moves just next to the rook
								grid[next[0]][next[1] + 1] = malloc(strlen(grid[prev[0]][prev[1]]));
								strcpy(grid[next[0]][next[1] + 1], grid[prev[0]][prev[1]]);
								// Rook moves just after the king
								grid[next[0]][next[1] + 2] = malloc(strlen(grid[next[0]][next[1]]));
								strcpy(grid[next[0]][next[1] + 2], grid[next[0]][next[1]]);
								// Clear both old cases
								grid[prev[0]][prev[1]] = malloc(strlen(" "));
								strcpy(grid[prev[0]][prev[1]], " ");
								grid[next[0]][next[1]] = malloc(strlen(" "));
								strcpy(grid[next[0]][next[1]], " ");
								break;
							case 7:
								// Moving right
								// King moves just next to the rook
								grid[next[0]][next[1] - 1] = malloc(strlen(grid[prev[0]][prev[1]]));
								strcpy(grid[next[0]][next[1] - 1], grid[prev[0]][prev[1]]);
								// Rook moves just after the king
								grid[next[0]][next[1] - 2] = malloc(strlen(grid[next[0]][next[1]]));
								strcpy(grid[next[0]][next[1] - 2], grid[next[0]][next[1]]);
								// Clear both old cases
								grid[prev[0]][prev[1]] = malloc(strlen(" "));
								strcpy(grid[prev[0]][prev[1]], " ");
								grid[next[0]][next[1]] = malloc(strlen(" "));
								strcpy(grid[next[0]][next[1]], " ");
								break;
							default:
								break;
						}
					} else {
						grid[next[0]][next[1]] = malloc(strlen(grid[prev[0]][prev[1]]));
						strcpy(grid[next[0]][next[1]], grid[prev[0]][prev[1]]);
						grid[prev[0]][prev[1]] = malloc(strlen(" "));
						strcpy(grid[prev[0]][prev[1]], " ");
					}
					if (strcmp(grid[next[0]][next[1]], pieces[1 - color][PAWN]) == 0 && next[0] == (iswhiteplayer ? 7 : 0)) {
						werase(input);
						box(input, 0, 0);
						mvwprintw(input, 0, 1, "Input");
						wrefresh(input);
						mvwprintw(input, 1, 1, "Promoted, you can change your pawn to something else : 1 = Queen, 2 = Rook, 3 = Bishop, 4 = Knight");
						mvwprintw(input, 2, 1, "Enter your choice here [1-4] > ");
						int promotedpiece = 0;
						while (promotedpiece == 0) {
							int c = wgetch(input);
							switch(c) {
								case '1':
									promotedpiece = 1;
									break;
								case '2':
									promotedpiece = 2;
									break;
								case '3':
									promotedpiece = 3;
									break;
								case '4':
									promotedpiece = 4;
									break;
								default:
									promotedpiece = 0;
									break;
							}
						}
//						scanf("%d", &promotedpiece);
//						while (promotedpiece < 1 || promotedpiece > 4) {
//							scanf("%d", &promotedpiece);
//						}
						grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(pieces[1 - color][promotedpiece]));
						strcpy(grid[cursor_pos_y][cursor_pos_x], pieces[1 - color][promotedpiece]);
					}
					if (gamemode == 1)
						iswhiteplayer = !iswhiteplayer;
					if (sockfd != 0) {
						send(sockfd, usermove, 4, 0);
						if (promotedpiece != 0) {
							send(sockfd, &promotedpiece, sizeof(int), 0);
							promotedpiece = 0;
						}
					}
				}
			} else { // Playing in arrows mode : player moves the cursor with arrows and confirms with spacebar
				mvwprintw(input, 2, 1, "Enter input (arrows style) > ");
				cbreak();
				wmove(input, 1, 1);
				int c;
				c = wgetch(input);
				switch(c) {
					case KEY_DOWN:
					case 'B':
						if (!iswhiteplayer) {
							if (cursor_pos_y != 7)
								cursor_pos_y++;
						} else {
							if (cursor_pos_y != 0)
								cursor_pos_y--;
						}
						break;
					case KEY_UP:
					case 'A':
						if (!iswhiteplayer) {
							if (cursor_pos_y != 0)
								cursor_pos_y--;
						} else {
							if (cursor_pos_y != 7)
								cursor_pos_y++;
						}
						break;
					case KEY_RIGHT:
					case 'C':
						if (cursor_pos_x != 7)
							cursor_pos_x++;
						break;
					case KEY_LEFT:
					case 'D':
						if (cursor_pos_x != 0)
							cursor_pos_x--;
						break;
					case 'q':
						usermove = "QUIT"; // Player quits, send the info to other player
						if (sockfd != 0)
							send(sockfd, usermove, 4, 0);
						doexit(0);
						break;
					case KEY_ENTER:
					case ' ':
						if (currentpiece[0] == 8 && currentpiece[1] == 8) {
							// Allow playing a piece only if it has available moves
							int movecount = 0;
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (is_move_allowed(iswhiteplayer ? WHITES : BLACKS, cursor_pos_y, cursor_pos_x, i, j)) {
										movecount++;
									}
								}
							}
							if (movecount != 0) {
								currentpiece[0] = cursor_pos_y;
								currentpiece[1] = cursor_pos_x;
							}
						} else {
							int color = (iswhiteplayer ? WHITES : BLACKS);
							if (currentpiece[0] == cursor_pos_y && currentpiece[1] == cursor_pos_x) {
								currentpiece[0] = 8;
								currentpiece[1] = 8;
							} else if (is_move_allowed(color, currentpiece[0], currentpiece[1], cursor_pos_y, cursor_pos_x)) {
								if (currentpiece[0] == (iswhiteplayer ? 0 : 8)) {
									switch(currentpiece[1]) {
										case 0:
											has_moved[0] = true;
											break;
										case 4:
											has_moved[1] = true;
											break;
										case 7:
											has_moved[2] = true;
											break;
										default:
											break;
									}
								}
								if (strcmp(grid[currentpiece[0]][currentpiece[1]], pieces[color][PAWN]) == 0 && currentpiece[1] != cursor_pos_x && strcmp(grid[cursor_pos_y][cursor_pos_x], " ") == 0) {
									// En passant
									lost_pieces[1 - color][lost_pieces_count[1 - color]] = grid[cursor_pos_y + (iswhiteplayer ? -1 : 1)][cursor_pos_x];
									grid[cursor_pos_y + (iswhiteplayer ? -1 : 1)][cursor_pos_x] = malloc(strlen(" "));
									strcpy(grid[cursor_pos_y + (iswhiteplayer ? -1 : 1)][cursor_pos_x], " ");
									lost_pieces_count[1 - color] += 1;
								}
								if (strcmp(grid[currentpiece[0]][currentpiece[1]], pieces[color][PAWN]) == 0 && abs(currentpiece[0] - cursor_pos_y) == 2) { // En passant
									en_passant.allowed[color] = true;
									en_passant.col[color] = currentpiece[1];
								}
								sendpacket = true;
								usermove[0] = (char) (currentpiece[1] + 65);
								usermove[1] = (char) (7 - currentpiece[0] + 49);
								usermove[2] = (char) (cursor_pos_x + 65);
								usermove[3] = (char) (7 - cursor_pos_y + 49);
								firstpart = 0;
								int color = iswhiteplayer ? BLACKS : WHITES;
								if (belongs(grid[cursor_pos_y][cursor_pos_x], color)) {
									if (strcmp(grid[cursor_pos_y][cursor_pos_x], pieces[color][KING]) == 0) {
										//clearscreen();
										erase();
										attron(COLOR_PAIR(FG_GREEN));
										mvprintw(0, 0, "You win");
										attroff(COLOR_PAIR(FG_GREEN));
										char win[4] = "LOST";
										send(sockfd, win, 4, 0);
										sleep(2);
										doexit(0);
									}
									lost_pieces[color][lost_pieces_count[color]] = grid[cursor_pos_y][cursor_pos_x];
									lost_pieces_count[color] += 1;
								}
								if (strcmp(grid[currentpiece[0]][currentpiece[1]], pieces[iswhiteplayer ? WHITES : BLACKS][KING]) == 0 && strcmp(grid[cursor_pos_y][cursor_pos_x], pieces[iswhiteplayer ? WHITES : BLACKS][ROOK]) == 0) {
									// Castling
									switch(cursor_pos_x) {
										case 0:
											// Moving left
											// King moves just next to the rook
											grid[cursor_pos_y][cursor_pos_x + 1] = malloc(strlen(grid[currentpiece[0]][currentpiece[1]]));
											strcpy(grid[cursor_pos_y][cursor_pos_x + 1], grid[currentpiece[0]][currentpiece[1]]);
											// Rook moves just after the king
											grid[cursor_pos_y][cursor_pos_x + 2] = malloc(strlen(grid[cursor_pos_y][cursor_pos_x]));
											strcpy(grid[cursor_pos_y][cursor_pos_x + 2], grid[cursor_pos_y][cursor_pos_x]);
											// Clear both old cases
											grid[currentpiece[0]][currentpiece[1]] = malloc(strlen(" "));
											strcpy(grid[currentpiece[0]][currentpiece[1]], " ");
											grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(" "));
											strcpy(grid[cursor_pos_y][cursor_pos_x], " ");
											break;
										case 7:
											// Moving right
											// King moves just next to the rook
											grid[cursor_pos_y][cursor_pos_x - 1] = malloc(strlen(grid[currentpiece[0]][currentpiece[1]]));
											strcpy(grid[cursor_pos_y][cursor_pos_x - 1], grid[currentpiece[0]][currentpiece[1]]);
											// Rook moves just after the king
											grid[cursor_pos_y][cursor_pos_x - 2] = malloc(strlen(grid[cursor_pos_y][cursor_pos_x]));
											strcpy(grid[cursor_pos_y][cursor_pos_x - 2], grid[cursor_pos_y][cursor_pos_x]);
											// Clear both old cases
											grid[currentpiece[0]][currentpiece[1]] = malloc(strlen(" "));
											strcpy(grid[currentpiece[0]][currentpiece[1]], " ");
											grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(" "));
											strcpy(grid[cursor_pos_y][cursor_pos_x], " ");
											break;
										default:
											break;
									}
								} else {
									grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(grid[currentpiece[0]][currentpiece[1]]));
									strcpy(grid[cursor_pos_y][cursor_pos_x], grid[currentpiece[0]][currentpiece[1]]);
									grid[currentpiece[0]][currentpiece[1]] = malloc(strlen(" "));
									strcpy(grid[currentpiece[0]][currentpiece[1]], " ");
								}
								if (strcmp(grid[cursor_pos_y][cursor_pos_x], pieces[1 - color][PAWN]) == 0 && cursor_pos_y == (iswhiteplayer ? 7 : 0)) {
									werase(input);
									box(input, 0, 0);
									mvwprintw(input, 0, 1, "Input");
									wrefresh(input);
									mvwprintw(input, 1, 1, "Promoted, you can change your pawn to something else : 1 = Queen, 2 = Rook, 3 = Bishop, 4 = Knight");
									mvwprintw(input, 2, 1, "Enter your choice here [1-4] > ");
									int promotedpiece = 0;
									while (promotedpiece == 0) {
										int c = wgetch(input);
										switch(c) {
											case '1':
												promotedpiece = 1;
												break;
											case '2':
												promotedpiece = 2;
												break;
											case '3':
												promotedpiece = 3;
												break;
											case '4':
												promotedpiece = 4;
												break;
											default:
												promotedpiece = 0;
												break;
										}
									}
//									printf("\rPromoted, you can change your pawn to something else :\n");
//									printf("\r1) Queen\n");
//									printf("\r2) Rook\n");
//									printf("\r3) Bishop\n");
//									printf("\r4) Knight\n");
//									printf("\r > ");
//									scanf("%d", &promotedpiece);
//									while (promotedpiece < 1 || promotedpiece > 4) {
//										scanf("%d", &promotedpiece);
//									}
									grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(pieces[1 - color][promotedpiece]));
									strcpy(grid[cursor_pos_y][cursor_pos_x], pieces[1 - color][promotedpiece]);
								}
								if (sockfd != 0) {
									send(sockfd, usermove, 4, 0);
									if (promotedpiece != 0) {
										send(sockfd, &promotedpiece, sizeof(int), 0);
										promotedpiece = 0;
									}
								}
								currentpiece[0] = 8;
								currentpiece[1] = 8;
								if (gamemode == 1)
									iswhiteplayer = !iswhiteplayer;
							}
						}
						break;
					case 'h':
						printhelp();
						break;
					default:
						break;
				}
				nocbreak();
			}
		}
		if (sendpacket) {
			// Display the history on the right
			int last_available = -1;
			for (int i = 0; i < lcount - 6; i++) {
				// Get if the history array has an empty line
				if (strcmp(history[i], "    ") == 0) {
					last_available = i;
					break;
				}
			}
			if (last_available == -1) { // The array is full
				for (int i = 1; i < lcount - 6; i++) { // Move every line of the array backwards
					history[i - 1] = malloc(4 * sizeof(char));
					for (int j =  0; j < 4; j++) {
						history[i - 1][j] = ' ';
						history[i - 1][j] = history[i][j];
					}
				}
				history[lcount - 7] = malloc(4 * sizeof(char));
				// Append the last move to the end of the array
				for (int i = 0; i < 4; i++) {
					history[lcount - 7][i] = ' ';
					history[lcount - 7][i] = usermove[i];
				}
			} else { // Just add the last move at the first free spot
				history[last_available] = malloc(4 * sizeof(char));
				for (int i = 0; i < 4; i++) {
					history[last_available][i] = ' ';
					history[last_available][i] = usermove[i];
				}
			}
			werase(hist);
			box(hist, 0, 0);
			mvwprintw(hist, 0, 1, "History");
			for (int i = 0; i < lcount - 6; i++) {
				if (strcmp(history[i], "    ") != 0) {
					mvwprintw(hist, 1 + i, 1, "%c", history[i][0]);
					mvwprintw(hist, 1 + i, 2, "%d", 9 - (int) (history[i][1] - '0'));
					mvwprintw(hist, 1 + i, 3, "%c", history[i][2]);
					mvwprintw(hist, 1 + i, 4, "%d", 9 - (int) (history[i][3] - '0'));
				}
			}
			wrefresh(hist);
		}
		printgrid();
		wrefresh(input);
		if (sockfd != 0 && sendpacket) { // Receiving input from the other player
			mvwprintw(input, 1, 1, "Waiting for other player");
			wrefresh(input);
			char* otherguymove = malloc(4 * sizeof(char));
			recv(sockfd, otherguymove, 4, 0);
			if (strcmp(otherguymove, "QUIT") == 0) { // The other player quit, exit now
				//clearscreen();
				erase();
				mvprintw(0, 0, "Other player has left");
				sleep(2);
				doexit(0);
			} else if (strcmp(otherguymove, "LOST") == 0) {
				//clearscreen();
				erase();
				attron(COLOR_PAIR(FG_RED));
				printf("You lost");
				attroff(COLOR_PAIR(FG_RED));
				sleep(2);
				doexit(0);
			}
			sendpacket = false;
			int color = iswhiteplayer ? WHITES : BLACKS;
			// Parse string to ints
			int prev[2] = {8 - (otherguymove[1] - '0'), (int) (otherguymove[0]) - 65};
			int next[2] = {8 - (otherguymove[3] - '0'), (int) (otherguymove[2]) - 65};
			if (belongs(grid[next[0]][next[1]], color)) {
				lost_pieces[color][lost_pieces_count[color]] = grid[next[0]][next[1]];
				lost_pieces_count[color] += 1;
			}
			if (strcmp(grid[prev[0]][prev[1]], pieces[1 - color][PAWN]) == 0 && abs(prev[0] - next[0]) == 2) { // Pawn moved 2 cases, allow en passant
				en_passant.allowed[1 - color] = true;
				en_passant.col[1 - color] = prev[1];
			}
			if (strcmp(grid[prev[0]][prev[1]], pieces[1 - color][PAWN]) == 0 && prev[1] != next[1] && strcmp(grid[next[0]][next[1]], " ") == 0) {
				// En passant
				lost_pieces[color][lost_pieces_count[color]] = grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]];
				grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]] = malloc(strlen(" "));
				strcpy(grid[next[0] + (iswhiteplayer ? -1 : 1)][next[1]], " ");
				lost_pieces_count[color] += 1;
			}
			if (strcmp(grid[prev[0]][prev[1]], pieces[1 - color][KING]) == 0 && strcmp(grid[next[0]][next[1]], pieces[1 - color][ROOK]) == 0) {
				// Castling
				switch(next[1]) {
					case 0:
						// Moving left
						// King moves just next to the rook
						grid[next[0]][next[1] + 1] = malloc(strlen(grid[prev[0]][prev[1]]));
						strcpy(grid[next[0]][next[1] + 1], grid[prev[0]][prev[1]]);
						// Rook moves just after the king
						grid[next[0]][next[1] + 2] = malloc(strlen(grid[next[0]][next[1]]));
						strcpy(grid[next[0]][next[1] + 2], grid[next[0]][next[1]]);
						// Clear both old cases
						grid[prev[0]][prev[1]] = malloc(strlen(" "));
						strcpy(grid[prev[0]][prev[1]], " ");
						grid[next[0]][next[1]] = malloc(strlen(" "));
						strcpy(grid[next[0]][next[1]], " ");
						break;
					case 7:
						// Moving right
						// King moves just next to the rook
						grid[next[0]][next[1] - 1] = malloc(strlen(grid[prev[0]][prev[1]]));
						strcpy(grid[next[0]][next[1] - 1], grid[prev[0]][prev[1]]);
						// Rook moves just after the king
						grid[next[0]][next[1] - 2] = malloc(strlen(grid[next[0]][next[1]]));
						strcpy(grid[next[0]][next[1] - 2], grid[next[0]][next[1]]);
						// Clear both old cases
						grid[prev[0]][prev[1]] = malloc(strlen(" "));
						strcpy(grid[prev[0]][prev[1]], " ");
						grid[next[0]][next[1]] = malloc(strlen(" "));
						strcpy(grid[next[0]][next[1]], " ");
						break;
					default:
						break;
				}
			} else if (strcmp(grid[next[0]][next[1]], pieces[1 - color][PAWN]) == 0 && abs(next[0] - prev[0]) == 2) {
				// Allow en passant
				en_passant.allowed[1 - color] = true;
				en_passant.col[1 - color] = next[1];
			} else {
				grid[next[0]][next[1]] = malloc(strlen(grid[prev[0]][prev[1]]));
				strcpy(grid[next[0]][next[1]], grid[prev[0]][prev[1]]);
				grid[prev[0]][prev[1]] = malloc(strlen(" "));
				strcpy(grid[prev[0]][prev[1]], " ");
			}
			if (strcmp(grid[next[0]][next[1]], pieces[1 - color][PAWN]) == 0 && next[0] == (iswhiteplayer ? 0 : 7)) {
				// Promotion
				int promotedpiece;
				recv(sockfd, &promotedpiece, sizeof(int), 0);
				grid[next[0]][next[1]] = malloc(strlen(pieces[1 - color][promotedpiece]));
				strcpy(grid[next[0]][next[1]], pieces[1 - color][promotedpiece]);
			}
			printgrid();
			wrefresh(game);
			// Display the history on the right
			int last_available = -1;
			for (int i = 0; i < lcount - 6; i++) {
				// Get if the history array has an empty line
				if (strcmp(history[i], "    ") == 0) {
					last_available = i;
					break;
				}
			}
			if (last_available == -1) { // The array is full
				for (int i = 1; i < lcount - 6; i++) { // Move every line of the array backwards
					history[i - 1] = malloc(4 * sizeof(char));
					for (int j =  0; j < 4; j++) {
						history[i - 1][j] = ' ';
						history[i - 1][j] = history[i][j];
					}
				}
				history[lcount - 7] = malloc(4 * sizeof(char));
				// Append the last move to the end of the array
				for (int i = 0; i < 4; i++) {
					history[lcount - 7][i] = ' ';
					history[lcount - 7][i] = otherguymove[i];
				}
			} else { // Just add the last move at the first free spot
				history[last_available] = malloc(4 * sizeof(char));
				for (int i = 0; i < 4; i++) {
					history[last_available][i] = ' ';
					history[last_available][i] = otherguymove[i];
				}
			}
			werase(hist);
			box(hist, 0, 0);
			mvwprintw(hist, 0, 1, "History");
			for (int i = 0; i < lcount - 6; i++) {
				if (strcmp(history[i], "    ") != 0) {
					mvwprintw(hist, 1 + i, 1, "%c", history[i][0]);
					mvwprintw(hist, 1 + i, 2, "%d", 9 - (int) (history[i][1] - '0'));
					mvwprintw(hist, 1 + i, 3, "%c", history[i][2]);
					mvwprintw(hist, 1 + i, 4, "%d", 9 - (int) (history[i][3] - '0'));
				}
			}
			wrefresh(hist);
			free(otherguymove);
		}
		sendpacket = false;
	}
}

// Show the user how to play
void tutorial(int argc, char* argv[]) {
	printgrid();
	refresh();
	printgrid();
	wrefresh(input);
	wrefresh(status);
	wrefresh(hist);
	wrefresh(game);
	refresh();
	pthread_t t;
	pthread_create(&t, NULL, timer, NULL);
	attron(COLOR_PAIR(FG_GREEN));
	char* msg0 = "Up here is the status bar, it shows you the time spent in the game";
	mvprintw(4, 0, msg0);
	refresh();
	getch();
	for (int i = 0; i < strlen(msg0); i++) {
		refresh();
		mvprintw(4, i, " ");
		refresh();
	}
	char* msg1 = "Down is the main playing area, this is where the game happens";
	mvprintw(mid_h - 12, mid_w - strlen(msg1) / 2, msg1);
	refresh();
	getch();
	for (int i = 0; i < strlen(msg1); i++) {
		mvprintw(mid_h - 12, mid_w - strlen(msg1) / 2 + i, " ");
	}
	char* msg2 = "This is the input area, where you will enter the move you want to play";
	mvprintw(lcount - 5, 0, msg2);
	refresh();
	getch();
	for (int i = 0; i < strlen(msg2); i++) {
		mvprintw(lcount - 5, i, " ");
	}
	char* msg3 = "This is the history bar, it will show you the history of moves";
	mvprintw(4, ccount - 20 - strlen(msg3), msg3);
	refresh();
	getch();
	for (int i = 0; i < strlen(msg3); i++) {
		mvprintw(4, ccount - 20 - strlen(msg3) + i, " ");
	}
	char* msg4 = "Now let's talk about the rules";
	isinmenu = true;
	erase();
	refresh();
	printgrid();
	wrefresh(game);
	mvprintw(0, 0, msg4);
	mvprintw(1, 0, "Your objective is to eat the king of the opposite color");
	refresh();
	getch();
	erase();
	refresh();
	printgrid();
	wrefresh(game);
	char* msg5 = "How to do so ? You have pieces with special moves";
	mvprintw(0, 0, msg5);
	mvprintw(1, 0, "- The pawn (all characters in lines 2 and 7) can move 1 up, or 1 up and 1 to the side to eat another piece, or 2 up if it has never moved before");
	mvprintw(2, 0, "- The rook (A1, H1, A8, H8) can move horizontally or vertically any number of cases but cannot go past another piece");
	mvprintw(3, 0, "- The knight (B1, G1, B8, G8) can move 1 horizontally and 2 vertically or 2 horizontally and 1 vertically");
	mvprintw(4, 0, "- The bishop (C1, F1, C8, F8) can move diagonally any number of cases but cannot go past another piece");
	mvprintw(5, 0, "- The queen (D1, D8) can move vertically and/or horizontally any number of cases but cannot go past another piece");
	mvprintw(6, 0, "- The king (E1, E8) can move 1 case around him");
	refresh();
	getch();
	erase();
	refresh();
	printgrid();
	wrefresh(game);
	char* msg6 = "Special moves, under certain conditions";
	mvprintw(0, 0, msg6);
	mvprintw(1, 0, "- If the king and a rook haved never moved during the game &");
	mvprintw(2, 0, "If the king is not in danger &");
	mvprintw(3, 0, "If every case between the king and the rook is empty and not in danger, ");
	mvprintw(4, 0, "Then, the king can move just next to the rook and the rook moves past the king at his new position");
	mvprintw(5, 0, "- If the opposite player moves his pawn 2 up &");
	mvprintw(6, 0, "If your pawn is just next its new position,");
	mvprintw(7, 0, "Then you can eat it by going diagonal (as if the opposite player only moved it 1 up)");
	mvprintw(8, 0, "- If your pawn reaches the last line of the board,");
	mvprintw(9, 0, "Then you can change it for whatever other piece you want");
	refresh();
	getch();
	erase();
	mvprintw(0, 0, "Now that you know everything, good luck");
	refresh();
	getch();
	//clearscreen();
	erase();
	// Relaunch the game when finished
	char* args[] = {argv[0], NULL};
	execvp(args[0],args);
	doexit(0);
}

void start(int mode, int argc, char* argv[]) {
	switch(mode) {
		case 2:
			gamemode = 2; // This is the tutorial
			tutorial(argc, argv);
			//clearscreen();
			erase();
			doexit(0);
			break;
		case 0:
			gamemode = 1; // Local multiplayer mode
			break;
		case 1:
			startaskserver(); // Network multiplayer mode
			gamemode = 0;
			break;
		default:
			break;
	}
	pthread_t time;
	pthread_create(&time, NULL, timer, NULL);
	wrefresh(status);
	wrefresh(game);
	wrefresh(lost_top);
	wrefresh(lost_bottom);
	wrefresh(input);
	wrefresh(hist);
	printgrid();
	// Starting game
	waitforevent();
}


// User chooses the playing mode
int menu() {
//	clearscreen();
	erase();
	int player_pos = 0;
	mvprintw(0, 0, "How would you like to play ? Use ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("arrow keys");
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" to move and press ");
	attron(COLOR_PAIR(FG_BLUE));
	printw("enter");
	attroff(COLOR_PAIR(FG_BLUE));
	printw(" to validate");
	//printf("How would you like to play ? Use \033[32marrow keys\033[0m to move and press \033[34menter\033[0m to validate\n");
	char* c1 = "Play with a friend (on this machine)";
	char* c2 = "Play with a friend (over the network)";
	char* c3 = "Learn how to play";
	char* list[3] = {c1, c2, c3};
	int color = 0;
	for (int i = 0; i < 3; i++) {
		if (player_pos == i) {
			color = FG_RED;
		} else {
			color = FG_WHITE;
		}
		attron(COLOR_PAIR(color));
		mvprintw(i + 1, 0, "%s", list[i]);
		attroff(COLOR_PAIR(color));
//		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
//	printf("\r");
//	system("/usr/bin/stty raw");
	int c;
	bool done = false;
	cbreak();
	while (!done) {
		c = getch();
		printf("\033[2K\033[A\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case KEY_DOWN:
			case 'B':
				if (player_pos != 2)
					player_pos++;
				break;
			case KEY_UP:
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			case KEY_ENTER:
			case '\n':
				done = true;
				break;
			default:
				break;
		}
		printf("\033[2K");
		int color;
		for (int i = 0; i < 3; i++) {
			if (player_pos == i) {
				color = FG_RED;
			} else {
				color = FG_WHITE;
			}
			attron(COLOR_PAIR(color));
			mvprintw(i + 1, 0, "%s", list[i]);
			attroff(COLOR_PAIR(color));
//			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
//		printf("\r");
	}
//	system("/usr/bin/stty cooked");
	return player_pos;
}

// User chooses the input mode
void select_playing_mode(int argc, char* argv[]) {
//	clearscreen();
	erase();
	int player_pos = 0;
	mvprintw(0, 0, "How would you like to control your inputs ? Use ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("arrow keys");
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" to move and press ");
	attron(COLOR_PAIR(FG_BLUE));
	printw("enter");
	attroff(COLOR_PAIR(FG_BLUE));
	printw(" to validate");
//	printf("How would you like to control your inputs ? Use \033[32marrow keys\033[0m to move and press \033[34menter\033[0m to validate\n");
	char* c1 = "Play with arrow keys";
	char* c2 = "Enter B1C3 style moves";
	char* list[2] = {c1, c2};
	int color = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
			color = FG_RED;
		} else {
			color = FG_WHITE;
		}
		attron(COLOR_PAIR(color));
		mvprintw(i + 1, 0, list[i]);
		attroff(COLOR_PAIR(color));
//		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
//	printf("\r");
//	system("/usr/bin/stty raw");
	int c;
	bool done = false;
	cbreak();
//	raw();
	keypad(stdscr, TRUE);
	while (!done) {
		c = getch();
		printf("\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case KEY_DOWN:
			case 'B':
				if (player_pos != 1)
					player_pos++;
				break;
			case KEY_UP:
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			case KEY_ENTER:
			case '\n':
				done = true;
				break;
			default:
				break;
		}
		printf("\033[2K");
		for (int i = 0; i < 2; i++) {
			if (player_pos == i) {
				color = FG_RED;
			} else {
				color = FG_WHITE;
			}
			attron(COLOR_PAIR(color));
			mvprintw(i + 1, 0, list[i]);
			attroff(COLOR_PAIR(color));
//			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
//		printf("\r");
	}
//	system("/usr/bin/stty cooked");
	keyboardmode = 1 - player_pos;
	// Do not show cursor position on B1C3 style input
	if (keyboardmode == LETTERSMODE) {
		cursor_pos_x = 8;
		cursor_pos_y = 8;
	}
	start(menu(), argc, argv);
}

int main (int argc, char* argv[]) {
	setlocale(LC_ALL, "");
	// Ugly way of having a global initial grid
    const char* grid2[8][8] = {
		{pieces[WHITES][ROOK], pieces[WHITES][KNIGHT], pieces[WHITES][BISHOP], pieces[WHITES][QUEEN], pieces[WHITES][KING], pieces[WHITES][BISHOP], pieces[WHITES][KNIGHT], pieces[WHITES][ROOK]},
		{pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN]},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN]},
		{pieces[BLACKS][ROOK], pieces[BLACKS][KNIGHT], pieces[BLACKS][BISHOP], pieces[BLACKS][QUEEN], pieces[BLACKS][KING], pieces[BLACKS][BISHOP], pieces[BLACKS][KNIGHT], pieces[BLACKS][ROOK]},
	};
	memcpy(&grid, &grid2, sizeof(grid));
//	clearscreen();
	erase();

	initscr();
	// Allow the use of arrow keys
	keypad(stdscr, TRUE);
	start_color();
	// Define color pairs
	init_pair(BG_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(BG_BLUE, COLOR_BLACK, COLOR_BLUE);
	init_pair(BG_RED, COLOR_BLACK, COLOR_RED);
	init_pair(BG_GREEN, COLOR_BLACK, COLOR_GREEN);
	init_pair(BG_YELLOW, COLOR_BLACK, COLOR_YELLOW);
	init_pair(BG_WHITE, COLOR_BLACK, COLOR_WHITE);
	init_pair(FG_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(FG_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(FG_RED, COLOR_RED, COLOR_BLACK);
	init_pair(FG_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(FG_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(FG_WHITE, COLOR_WHITE, COLOR_BLACK);
	mvprintw(0, 0, "This game is best played with a big font, you should resize your terminal font with ");
	attron(COLOR_PAIR(FG_GREEN));
	printw("Ctrl+Shift++");
	attroff(COLOR_PAIR(FG_GREEN));
	printw(" if your terminal supports it");
	mvprintw(1, 0, "Press any key when you are ready");
//	printf("\rThis game is best played with a big font, you should resize your terminal font with \033[32mCtrl+Shift++\033[0m\n\rPress any key when you are ready\n");
	getch();

	// Get the size of the terminal
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	lcount = w.ws_row;
	ccount = w.ws_col;
	char* move_str = "    ";
	history = malloc((lcount - 6) * sizeof(move_str));
	for (int i = 0; i < lcount - 6; i++) {
		history[i] = malloc(4 * sizeof(char));
		strcpy(history[i], move_str);
	}
	mid_h = (lcount % 2 == 0 ? lcount / 2 : (lcount + 1) / 2);
	mid_w = (ccount % 2 == 0 ? ccount / 2 : (ccount + 1) / 2);
	// Define the game HUD
	status = newwin(4, ccount, 0, 0);
	game = newwin(12, 12, mid_h - 6, mid_w - 6);
	lost_top = newwin(4, 12, mid_h - 10, mid_w - 6);
	lost_bottom = newwin(4, 12, mid_h + 6, mid_w - 6);
	input = newwin(4, ccount - 20, lcount - 4, 0);
	hist = newwin(lcount - 4, 20, 4, ccount - 20);
	// Display a rectangle around each window
	box(status, 0, 0);
	box(game, 0, 0);
	box(lost_top, 0, 0);
	box(lost_bottom, 0, 0);
	box(input, 0, 0);
	box(hist, 0, 0);
	// Title for the boxes
	mvwprintw(status, 0, 1, "Status bar");
	mvwprintw(game, 0, 1, "Grid");
	mvwprintw(lost_top, 0, 1, "Lost pieces");
	mvwprintw(lost_bottom, 0, 1, "Won pieces");
	mvwprintw(input, 0, 1, "Input");
	mvwprintw(hist, 0, 1, "History");

	select_playing_mode(argc, argv);
//	start(menu(), argc, argv);

	// Close the sockets if we were playing online
	if (sockfd != 0)
		close(sockfd);
	if (new_fd != 0)
		close(new_fd);
}

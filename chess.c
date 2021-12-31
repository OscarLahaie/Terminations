/**********************************************/
/*     Chess - a really simple chess game     */
/* Made by srgoti <fl.valentin1904@gmail.com> */
/**********************************************/

// All the system("/usr/bin/stty ...") calls control the way input is entered in the terminal

// TODO: Add the En passant and Castling moves
// TODO: Parse the input to check if the move is allowed

// TODO: Is everything needed here ?
#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <regex.h>

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
#define BG_BLACK 40
#define BG_RED 41
#define BG_GREEN 42
#define BG_ORANGE 43
#define BG_BLUE 44
#define BG_PURPLE 45
#define BG_TEAL 46
#define BG_WHITE 47

#define FG_BLACK 30
#define FG_RED 31
#define FG_GREEN 32
#define FG_ORANGE 33
#define FG_BLUE 34
#define FG_PURPLE 35
#define FG_TEAL 36
#define FG_WHITE 37

// Define playing control mode
#define LETTERSMODE 0
#define ARROWSMODE 1

// Define the grid, might delete later, IDK
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7

//int globalscore = 0;
//char letters[8] = "ABCDEFGH";
char* pieces[2][6] = {
	{"♚", "♛", "♜", "♝", "♞", "♟"},
	{"♔", "♕", "♖", "♗", "♘", "♙"},
};
char* lost_pieces[2][16] = {{""}};
int lost_pieces_count[2] = {0};
int lcount;
int ccount;
char* grid[8][8] = {{""}};
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

// This will be useful to print the inverted grid for the black pieces player
int invertedbuffer[8][8] = {{0}};

// Check if provided piece is part of team `color`
bool belongs(char* piece, int color) {
	for (int i = 0; i < 6; i++) {
		if (strcmp(piece, pieces[color][i]) == 0)
			return true;
	}
	return false;
}

bool is_move_allowed(int color, int start_y, int start_x, int finish_y, int finish_x) {
	// Would be better w/ a switch case but not available as strings are pointers
	if (strcmp(grid[start_y][start_x], pieces[color][KING]) == 0) { // Allowed moves for the king
		if (abs(start_x - finish_x) <= 1 && abs(start_y - finish_y) <= 1) { // Move must be <= 1 cases
			if (belongs(grid[finish_y][finish_x], color)) { // Allow move if destination is not a same color piece
				return false;
			} else {
				return true;
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
			for (int i = 1; i < ((abs(finish_y - start_y) = 0) ? abs(finish_x - start_x) : abs(finish_y - start_y)); i++) {
//			for (int i = 1; i < abs(finish_y - start_y); i++) {
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
			for (int i = 1; i < abs(finish_y - start_y); i++) {
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
					return true;
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
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else {
			if (finish_x == start_x && finish_y - start_y == 2 && start_y == check) { // Allow two cases in front it pawn is at its start point
				if (strcmp(grid[finish_y][finish_x], " ") == 0) {
					return true;
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

void clearscreen() {
	fflush(stdout);
	printf("\033[1;1H\033[2J");
}

// Player asked for help
// TODO: Install this message
void printhelp() {
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A\033[2K");
	}
	printf("This game is about...\n\r\033[2mControls\033[0m : ..., ESC to return to game");
	int c;
	while ((c = getchar()) != 27) {}
	clearscreen();
}


// User called exit, cleanup
void doexit(int err) {
	clearscreen();
	system("reset");
	if (sockfd != 0)
		close(sockfd);
	if (new_fd != 0)
		close(new_fd);
	exit(err);
}

// Print the top (status) bar
void printbar() {
	char* left = malloc(22);
	sprintf(left, "Chess v1, %s player", iswhiteplayer ? "white" : "black");
	printf("\033[30;47m%s%*s\033[0m\n", left, (int) (ccount - strlen(left)), "TEST");
}

// Display the game grid on screen
// The grid should display the current player's pieces at the bottom
// Hence the conditions and (1)
void printgrid() {
	printf("\r");
	printbar();
	printf("\r");
	for (int i = 0; i < lost_pieces_count[iswhiteplayer ? WHITES : BLACKS]; i++) {
		printf("%s", lost_pieces[iswhiteplayer ? WHITES : BLACKS][i]);
		if (i == 7)
			printf("\n\r");
	}
	printf("\n\r");
	printf(" \033[31mABCDEFGH\033[0m\n\r");
	for (int i = 0; i < 8; i++) {
		if (!iswhiteplayer)
			printf("\033[31m%d\033[0m", i + 1);
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
						casecolor = BG_ORANGE;
					} else if (is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], i, j)) { // Player is choosing a destination to its piece
						casecolor = BG_BLUE;
					} else {
						casecolor = BG_RED;
					}
				}
			} else {
				if (i == currentpiece[0] && j == currentpiece[1]) {
					casecolor = BG_ORANGE;
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
							casecolor = 0;
						}
					}
				}
			}
// Beginning of (1)
			invertedbuffer[i][j] = casecolor;
			if (!iswhiteplayer) {
				printf("\033[%dm%s\033[0m", casecolor, grid[i][j]);
			}
		}
		if (!iswhiteplayer) {
			printf("\033[31m%d\033[0m", i + 1);
			printf("\n\r");
		}
	}
	if (iswhiteplayer) {
		for (int i = 7; i >= 0; i--) {
			printf("\033[31m%d\033[0m", i + 1);
			for (int j = 0; j < 8; j++) {
				printf("\033[%dm%s\033[0m", invertedbuffer[i][j], grid[i][j]);
			}
			printf("\033[31m%d\033[0m", i + 1);
			printf("\n");
		}
	}
	printf(" \033[31mABCDEFGH\033[0m\n\r");
// End of (1)
	for (int i = 0; i < lost_pieces_count[iswhiteplayer ? BLACKS : WHITES]; i++) {
		printf("%s", lost_pieces[iswhiteplayer ? BLACKS : WHITES][i]);
		if (i == 7)
			printf("\n\r");
	}
}

// Client connect code
// Most of the code is from a document at http://beej.us/guide/bgnet/
void startconnect() {
	regex_t regex;
	regcomp(&regex, "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$", 0); // Regex for an IP Address, stolen on S.O.
	clearscreen();
	printf("\rEnter an IP address, then press \033[34menter\033[0m to validate\n");
	char* address = malloc(16 * sizeof(char));
	scanf("%s", address);
	while (!regexec(&regex, address, 0, NULL, 0)) {
		clearscreen();
		printf("\rEnter an IP address, then press \033[34menter\033[0m to validate\n");
		scanf("%s", address);
	}
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(address, "4050", &hints, &res); // Open a connection at address:4050
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	clearscreen();
	printf("\rTrying to connect\n");
	struct timeval timeout;
	timeout.tv_sec  = 5;  // Timeout after 5 seconds
	timeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		clearscreen();
		printf("\r\033[31mFailed to connect to host\033[0m\n");
		sleep(2);
		doexit(1);
	}
	clearscreen();
	printf("\rConnected\nWaiting for server to choose color\n");
	// Server chooses a color, returns the other to client
	int color2;
	recv(sockfd, &color2, sizeof(int), 0);
	clearscreen();
	iswhiteplayer = color2 == WHITES;
}

// Server host code
// Cf same guide
void startserver() {
	struct addrinfo hints, *res;
	struct sockaddr_in client;
	socklen_t addr_size;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, "4050", &hints, &res);
	new_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	bind(new_fd, res->ai_addr, res->ai_addrlen);
	listen(new_fd, 1);
	clearscreen();
	printf("\rWaiting for someone to connect\n");
	addr_size = sizeof(client);
	sockfd = accept(new_fd, (struct sockaddr *) &client, &addr_size);
	clearscreen();
	struct hostent *hostName;
	struct in_addr ipv4addr;
	inet_pton(AF_INET, inet_ntoa(client.sin_addr), &ipv4addr);
	hostName = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	printf("\rReceived connection from \"%s\", IP address : %s\n", hostName->h_name, inet_ntoa(client.sin_addr));
	// Server chooses a color
	printf("What color would you like to play ? Use \033[32marrow keys\033[0m to move and press \033[34mspace\033[0m to validate\n");
	char* c1 = "Play as white";
	char* c2 = "Play as black";
	char* list[2] = {c1, c2};
	int bgcolor = 0;
	int fgcolor = 0;
	int player_pos = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
			bgcolor = FG_WHITE;
			fgcolor = BG_RED;
		} else {
			bgcolor = BG_WHITE;
			fgcolor = FG_RED;
		}
		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
	printf("\r");
	system("/usr/bin/stty raw");
	int c;
	while ((c = getchar()) != ' ') {
		printf("\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case 'B':
				if (player_pos != 1)
					player_pos++;
				break;
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			default:
				break;
		}
		printf("\033[2K");
		for (int i = 0; i < 2; i++) {
			if (player_pos == i) {
				bgcolor = FG_WHITE;
				fgcolor = BG_RED;
			} else {
				bgcolor = BG_WHITE;
				fgcolor = FG_RED;
			}
			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
		printf("\r");
	}
	clearscreen();
	system("/usr/bin/stty cooked");
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
	clearscreen();
}

// We are playing in LAN mode, user has to choose to be server or client (doesn't change much in game)
void startaskserver() {
	clearscreen();
	int player_pos = 0;
	printf("Who will be hosting the game ? Use \033[32marrow keys\033[0m to move and press \033[34mspace\033[0m to validate\n");
	char* c1 = "Host the game";
	char* c2 = "Connect to a host";
	char* list[2] = {c1, c2};
	int bgcolor = 0;
	int fgcolor = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
			bgcolor = FG_WHITE;
			fgcolor = BG_RED;
		} else {
			bgcolor = BG_WHITE;
			fgcolor = FG_RED;
		}
		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
	printf("\r");
	system("/usr/bin/stty raw");
	int c;
	while ((c = getchar()) != ' ') {
		printf("\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case 'B':
				if (player_pos != 1)
					player_pos++;
				break;
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			default:
				break;
		}
		printf("\033[2K");
		for (int i = 0; i < 2; i++) {
			if (player_pos == i) {
				bgcolor = FG_WHITE;
				fgcolor = BG_RED;
			} else {
				bgcolor = BG_WHITE;
				fgcolor = FG_RED;
			}
			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
		printf("\r");
	}
	system("/usr/bin/stty cooked");
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

// Wait for any playing event
void* waitforevent() {
	char* usermove = malloc(4 * sizeof(char));
	while (true) {
		skipturn = false;
		if (sockfd != 0 && !iswhiteplayer && firstmove == 0) {
			// If we are not white player and in lan, wait for the other player to start
			printf("\033[A\033[2K\r\033[31mWaiting for other player\033[0m\n\033[2K");
			char* otherguymove = malloc(4 * sizeof(char));
			recv(sockfd, otherguymove, 4, 0);
			if (strcmp(otherguymove, "QUIT") == 0) { // The other player quit, exit now
				clearscreen();
				printf("\rOther player has left\n");
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
			free(otherguymove);
			firstmove = 1;
		} else {
			printf("\r\033[32mYour turn to play\033[0m\n");
			if (keyboardmode == LETTERSMODE) { // In this mode, player moves by entering grid values, e.g : B1C3 on the first turn will move the white left knight to the C3 case
				int prev[2] = {8, 8};
				int next[2] = {8, 8};
				system("/usr/bin/stty cooked");
				while (strcmp(usermove, "WAIT") != 0 && ((prev[0] == 8 && prev[1] == 8 && next[0] == 8 && next[1] == 8) || !is_move_allowed(iswhiteplayer ? WHITES : BLACKS, prev[0], prev[1], next[0], next[1]))) {
					if (prev[0] != 8 && prev[1] != 8 && next[0] != 8 && next[1] != 8) {
						printf("\r\033[A\033[2K");
						printf("\033[31mInvalid move\033[0m\n");
						sleep(2);
						printf("\r\033[A\033[2K");
					}
					printf("\rEnter input (B1C3 style) > ");
					scanf("%s", usermove);
					if (strcmp(usermove, "m") == 0) { // User changes playing mode, inform the other player and skip this loop
						keyboardmode = ARROWSMODE;
						usermove = "WAIT";
						if (sockfd != 0)
							send(sockfd, usermove, 4, 0);
					} else if (strcmp(usermove, "h") == 0) {
						usermove = "WAIT";
						if (sockfd != 0)
							send(sockfd, usermove, 4, 0);
						printhelp();
					} else if (strcmp(usermove, "q") == 0) { // Player quits, send the info to the other player
						usermove = "QUIT";
						if (sockfd != 0)
							send(sockfd, usermove, 4, 0);
						doexit(0);
					} else {
						regex_t regex;
						regcomp(&regex, "^[A-G][1-8][A-G][1-8]$", 0);
						while (regexec(&regex, usermove, 0, NULL, 0) != 0) { // Check if the input is valid
							printf("\033[A");
							printf("\r\033[2KInvalid move\n");
							sleep(2);
							printf("\033[A");
							printf("\rEnter input (B1C3 style) > ");
							scanf("%s", usermove);
							if (strcmp(usermove, "m") == 0) {
								keyboardmode = ARROWSMODE;
								usermove = "WAIT";
								break;
							} else if (strcmp(usermove, "h") == 0) {
								usermove = "WAIT";
								printhelp();
								break;
							} else if (strcmp(usermove, "q") == 0) { // Player quits, send the info to the other player
								usermove = "QUIT";
								doexit(0);
							}
						}
						if (strcmp(usermove, "WAIT") != 0) {
							prev[0] = 8 - (usermove[1] - '0');
							prev[1] = (int) (usermove[0]) - 65;
							next[0] = 8 - (usermove[3] - '0');
							next[1] = (int) (usermove[2]) - 65;
						}
					}
				}
				if (strcmp(usermove, "WAIT") != 0) {
					int color = iswhiteplayer ? BLACKS : WHITES;
					if (belongs(grid[next[0]][next[1]], color)) {
						lost_pieces[color][lost_pieces_count[color]] = grid[next[0]][next[1]];
						lost_pieces_count[color] += 1;
					}
					grid[next[0]][next[1]] = malloc(strlen(grid[prev[0]][prev[1]]));
					strcpy(grid[next[0]][next[1]], grid[prev[0]][prev[1]]);
					grid[prev[0]][prev[1]] = malloc(strlen(" "));
					strcpy(grid[prev[0]][prev[1]], " ");
					if (strcmp(grid[next[0]][next[1]], pieces[1 - color][PAWN]) == 0 && next[0] == (iswhiteplayer ? 7 : 0)) {
						// TODO: Promotion
						printf("Promoted\n");
					}
					if (gamemode == 1)
	//					nextplayer = 1 - nextplayer;
						iswhiteplayer = !iswhiteplayer;
				}
				if (sockfd != 0)
					send(sockfd, usermove, 4, 0);
			} else { // Playing in arrows mode : player moves the cursor with arrows and confirms with spacebar
				printf("\rEnter input (arrows style) > ");
				system("/usr/bin/stty raw");
				int c;
				c = getchar();
				switch(c) {
					case 'B':
						if (!iswhiteplayer) {
							if (cursor_pos_y != 7)
								cursor_pos_y++;
						} else {
							if (cursor_pos_y != 0)
								cursor_pos_y--;
						}
						break;
					case 'A':
						if (!iswhiteplayer) {
							if (cursor_pos_y != 0)
								cursor_pos_y--;
						} else {
							if (cursor_pos_y != 7)
								cursor_pos_y++;
						}
						break;
					case 'C':
						if (cursor_pos_x != 7)
							cursor_pos_x++;
						break;
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
					case ' ':
						// TODO: Check if the input is allowed (e.g right color piece)
						if (currentpiece[0] == 8 && currentpiece[1] == 8) {
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
//								firstpart += cursor_pos_y * 1000 + cursor_pos_x * 100;
							}
						} else {
							if (currentpiece[0] == cursor_pos_y && currentpiece[1] == cursor_pos_x) {
//								firstpart = 0;
								currentpiece[0] = 8;
								currentpiece[1] = 8;
							} else if (is_move_allowed(iswhiteplayer ? WHITES : BLACKS, currentpiece[0], currentpiece[1], cursor_pos_y, cursor_pos_x)) {
//								firstpart += cursor_pos_y * 10 + cursor_pos_x;
								sendpacket = true;
								char usermove[4] = {
									(char) (currentpiece[1] + 65), 
									(char) (8 - currentpiece[0] + 48), 
									(char) (cursor_pos_x + 65),
									(char) (8 - cursor_pos_y + 48), 
								};
								firstpart = 0;
								int color = iswhiteplayer ? BLACKS : WHITES;
								if (belongs(grid[cursor_pos_y][cursor_pos_x], color)) {
									lost_pieces[color][lost_pieces_count[color]] = grid[cursor_pos_y][cursor_pos_x];
									lost_pieces_count[color] += 1;
								}
								grid[cursor_pos_y][cursor_pos_x] = malloc(strlen(grid[currentpiece[0]][currentpiece[1]]));
								strcpy(grid[cursor_pos_y][cursor_pos_x], grid[currentpiece[0]][currentpiece[1]]);
								grid[currentpiece[0]][currentpiece[1]] = malloc(strlen(" "));
								strcpy(grid[currentpiece[0]][currentpiece[1]], " ");
								if (strcmp(grid[cursor_pos_y][cursor_pos_x], pieces[1 - color][PAWN]) == 0 && cursor_pos_y == (iswhiteplayer ? 7 : 0)) {
									printf("Promoted\n");
								}
								if (sockfd != 0)
									send(sockfd, usermove, 4, 0);
								currentpiece[0] = 8;
								currentpiece[1] = 8;
								if (gamemode == 1)
									iswhiteplayer = !iswhiteplayer;
							}
						}
						break;
					case 'm':
						firstpart = 0;
						keyboardmode = LETTERSMODE;
						break;
					case 'h':
						printhelp();
						break;
					default:
						break;
				}
				system("/usr/bin/stty cooked");
			}
		}
		clearscreen();
		printgrid();
		if (sockfd != 0 && sendpacket) { // Receiving input from the other player
			printf("\033[A\033[2K\r\033[31mWaiting for other player\033[0m\n\033[2K");
			char* otherguymove = malloc(4 * sizeof(char));
			recv(sockfd, otherguymove, 4, 0);
			if (strcmp(otherguymove, "QUIT") == 0) { // The other player quit, exit now
				clearscreen();
				printf("\rOther player has left\n");
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
			clearscreen();
			printgrid();
		}
	}
}

void start(int mode) {
	switch(mode) {
		case 2:
			gamemode = 2; // Against the 'AI' mode
//			startagainstai();
			break;
		case 0:
			gamemode = 1; // Local multiplayer mode
//			startlocal();
			break;
		case 1:
			startaskserver(); // Network multiplayer mode
			gamemode = 0;
			break;
		default:
			break;
	}
	printgrid();
	// TODO: Check if I really need a thread here
	pthread_t listenforinput;
	pthread_create(&listenforinput, NULL, waitforevent, NULL);
	pthread_join(listenforinput, NULL);
}


// User chooses the playing mode
int menu() {
	clearscreen();
	int player_pos = 0;
	printf("How would you like to play ? Use \033[32marrow keys\033[0m to move and press \033[34mspace\033[0m to validate\n");
	char* c1 = "Play with a friend (on this machine)";
	char* c2 = "Play with a friend (over the network)";
	char* c3 = "Play agaisnt the computer";
	char* list[3] = {c1, c2, c3};
	int bgcolor = 0;
	int fgcolor = 0;
	for (int i = 0; i < 3; i++) {
		if (player_pos == i) {
			bgcolor = FG_WHITE;
			fgcolor = BG_RED;
		} else {
			bgcolor = BG_WHITE;
			fgcolor = FG_RED;
		}
		printf("\r\033[%d;%dm%s\033[0m\n", fgcolor, bgcolor, list[i]);
	}
	printf("\r");
	system("/usr/bin/stty raw");
	int c;
	while ((c = getchar()) != ' ') {
		printf("\033[2K\033[A\033[2K\033[A\033[2K\033[A");
		switch(c) {
			case 'B':
				if (player_pos != 2)
					player_pos++;
				break;
			case 'A':
				if (player_pos != 0)
					player_pos--;
				break;
			default:
				break;
		}
		printf("\033[2K");
		for (int i = 0; i < 3; i++) {
			if (player_pos == i) {
				bgcolor = FG_WHITE;
				fgcolor = BG_RED;
			} else {
				bgcolor = BG_WHITE;
				fgcolor = FG_RED;
			}
			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
		printf("\r");
	}
	system("/usr/bin/stty cooked");
	return player_pos;
}

int main () {
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
	clearscreen();

	printf("This game is best played with a big font, you should resize your terminal font with \033[%dmCtrl+Shift++\033[0m\nPress \033[%dmEnter\033[0m when you are ready\n", 32, 34);
	getchar();

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	lcount = w.ws_row;
	ccount = w.ws_col;

	int mode = menu();
	start(mode);

	// Close the sockets if we were playing online
	if (sockfd != 0)
		close(sockfd);
	if (new_fd != 0)
		close(new_fd);
}

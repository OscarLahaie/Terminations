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
#include <termios.h>

#define KING 0
#define QUEEN 1
#define ROOK 2
#define BISHOP 3
#define KNIGHT 4
#define PAWN 5

#define BLACKS 0
#define WHITES 1

#define LETTERSMODE 0
#define ARROWSMODE 1

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7

#define SERVER 0
#define CLIENT 1

int globalscore = 0;
char* pieces[2][6] = {
	{"♔", "♕", "♖", "♗", "♘", "♙"},
	{"♚", "♛", "♜", "♝", "♞", "♟"}
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
//int currentdirection = SERVERTOCLIENT;
bool iswhiteplayer;
int servertoclient = 0;
int firstmove = 0;
bool skipturn = 0;
int firstpart = 0;
bool sendpacket = false;

bool iswhitepiece(char* piece) {
	for (int i = 0; i < 6; i++) {
		if (piece == pieces[WHITES][i])
			return true;
	}
	return false;
}

void clearscreen() {
	fflush(stdout);
	printf("\033[1;1H\033[2J");
}

void doexit(int err) {
	clearscreen();
	system("reset");
	exit(err);
}

void printgrid() {
	printf("\r");
	for (int i = 0; i < lost_pieces_count[WHITES]; i++) {
		printf("%s", lost_pieces[WHITES][i]);
		if (i == 7)
			printf("\n\r");
	}
	printf("\n\r");
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
//			printf("%s\n", grid2[i][j]);
//			printf("Done\n");
			if (strcmp(grid[i][j], " ") != 0)
				printf("%s", grid[i][j]);
			else {
				if ((i + j) % 2 == 0)
					printf("\033[47m%s\033[0m", grid[i][j]);
				else
					printf("\033[40m%s\033[0m", grid[i][j]);
			}
		}
		printf("\n\r");
	}
	for (int i = 0; i < lost_pieces_count[BLACKS]; i++) {
		printf("%s", lost_pieces[BLACKS][i]);
		if (i == 7)
			printf("\n\r");
	}
	printf("\n");
}

void startconnect() {
	regex_t regex;
	regcomp(&regex, "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$", 0);
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
//	struct sockaddr_storage connector_address;
//	socklen_t addr_size;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(address, "4050", &hints, &res);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	clearscreen();
	printf("\rTrying to connect\n");
	struct timeval timeout;
	timeout.tv_sec  = 7;  // after 7 seconds connect() will timeout
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
//	int c;
//	recv(sockfd, &c, sizeof(c), 0);
//	printf("\r%d\n", c);
//	client = CLIENT;
//	char* color = malloc(5 * sizeof(char));
	int color2;
//	recv(sockfd, color, 5, 0);
	recv(sockfd, &color2, sizeof(int), 0);
	clearscreen();
//	printf("%d\n", color2);
//	printf("\rPlayer color : %s\n", color2 == 0 ? "Black" : "White");
	iswhiteplayer = color2 == 1;
//	exit(0);
//    close(sockfd);
}

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
//	int c = 50;
//	send(sock, &c, sizeof(c), 0);
//	client = SERVER;
//	close(new_fd);
//	close(sockfd);
	printf("What color would you like to play ? Use \033[32marrow keys\033[0m to move and press \033[34mspace\033[0m to validate\n");
	char* c1 = "Play as white";
	char* c2 = "Play as black";
	char* list[2] = {c1, c2};
	int bgcolor = 0;
	int fgcolor = 0;
	int player_pos = 0;
	for (int i = 0; i < 2; i++) {
		if (player_pos == i) {
			bgcolor = 37;
			fgcolor = 41;
		} else {
			bgcolor = 47;
			fgcolor = 31;
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
				bgcolor = 37;
				fgcolor = 41;
			} else {
				bgcolor = 47;
				fgcolor = 31;
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
			iswhiteplayer = true;
			sendblack2 = 0;
			break;
		case 1:
			iswhiteplayer = false;
			sendblack2 = 1;
			break;
		default:
			break;
	}
//	send(sockfd, sendblack, 5, 0);
	send(sockfd, &sendblack2, sizeof(int), 0);
	clearscreen();
//	printf("\rPlayer color : %s\n", iswhiteplayer ? "White" : "Black");
}

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
			bgcolor = 37;
			fgcolor = 41;
		} else {
			bgcolor = 47;
			fgcolor = 31;
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
				bgcolor = 37;
				fgcolor = 41;
			} else {
				bgcolor = 47;
				fgcolor = 31;
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

void* waitforevent() {
	while (true) {
		skipturn = false;
		if (sockfd != 0 && !iswhiteplayer && firstmove == 0) {
			char* othermove = malloc(4 * sizeof(char));
			recv(sockfd, othermove, 4, 0);
			firstmove = 1;
			free(othermove);
		} else {
			printf("\033[33mYour turn to play\033[0m\n");
			if (keyboardmode == LETTERSMODE) {
				system("/usr/bin/stty cooked");
				char* usermove = malloc(4 * sizeof(char));
				scanf("%s", usermove);
				if (strcmp(usermove, "m") == 0) {
					keyboardmode = ARROWSMODE;
					usermove = "WAIT";
					send(sockfd, usermove, 4, 0);
				} else {
					regex_t regex;
					regcomp(&regex, "^[A-G][1-8][A-G][1-8]$", 0);
					while (!regexec(&regex, usermove, 0, NULL, 0) && !skipturn) {
//						clearscreen();
						printf("\033[A");
						printf("\rInvalid move");
						sleep(2);
						printf("\033[2K");
						scanf("%s", usermove);
						if (strcmp(usermove, "m") == 0) {
							keyboardmode = ARROWSMODE;
							skipturn = true;
							usermove = "WAIT";
						}
					}
					send(sockfd, usermove, 4, 0);
					free(usermove);
				}
			} else {
				printf("Waiting for input\n");
				system("/usr/bin/stty raw");
//				printf("%c", getch());
				int c;
				c = getchar();
//				putchar(c);
				switch(c) {
					case 'A':
						if (cursor_pos_y != 7)
							cursor_pos_y++;
						break;
					case 'B':
						if (cursor_pos_y != 0)
							cursor_pos_y--;
						break;
					case 'C':
						if (cursor_pos_x != 7)
							cursor_pos_x++;
						break;
					case 'D':
						if (cursor_pos_x != 7)
							cursor_pos_x--;
						break;
					case 'q':
						char* quit = "QUIT";
						send(sockfd, quit, 4, 0);
					case ' ':
						if (firstpart == 0) {
							firstpart += cursor_pos_y * 1000 + cursor_pos_x * 100;
						} else {
							firstpart += cursor_pos_y * 10 + cursor_pos_x;
							sendpacket = true;
							char* mymove = malloc(4 * sizeof(char));
							sprintf(mymove, "%d", firstpart);
							send(sockfd, mymove, 4, 0);
							firstpart = 0;
						}
						break;
					case 'm':
						firstpart = 0;
						keyboardmode = LETTERSMODE;
						break;
					default:
						break;
				}
			}
		}
		if (sockfd != 0 && sendpacket) {
			printf("\033[31mWaiting for other player\033[0m\n");
			char* otherguymove = malloc(4 * sizeof(char));
			recv(sockfd, otherguymove, 4, 0);
			if (strcmp(otherguymove, "QUIT") == 0) {
				clearscreen();
				printf("\rOther player has left\n");
				sleep(2);
				doexit(0);
			}
			sendpacket = false;
//			if (grid[
		}
		clearscreen();
		printgrid();
	}
}

void start(int mode) {
	switch(mode) {
		case 2:
			gamemode = 2;
//			startagainstia();
			break;
		case 0:
			gamemode = 1;
//			startlocal();
			break;
		case 1:
			startaskserver();
			gamemode = 0;
			break;
		default:
			break;
	}
	printgrid();
	pthread_t listenforinput;
	pthread_create(&listenforinput, NULL, waitforevent, NULL);
	pthread_join(listenforinput, NULL);
}

void printhelp() {
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[A\033[2K");
	}
	printf("This game is about...\n\r\033[2mControls\033[0m : ..., ESC to return to game");
	int c;
	while ((c = getchar()) != 27) {}
	for (int i = 0; i < lcount; i++) {
		printf("\r\033[B\033[2K");
	}
}

/*void* printinput() {
	system("/usr/bin/stty raw");
	int c;
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
	system("/usr/bin/stty cooked");
}*/

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
			bgcolor = 37;
			fgcolor = 41;
		} else {
			bgcolor = 47;
			fgcolor = 31;
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
				bgcolor = 37;
				fgcolor = 41;
			} else {
				bgcolor = 47;
				fgcolor = 31;
			}
			printf("\r\033[%d;%dm%s\n\033[0m", fgcolor, bgcolor, list[i]);
		}
		printf("\r");
	}
	system("/usr/bin/stty cooked");
	return player_pos;
}

int main () {
    const char* grid2[8][8] = {
		{pieces[BLACKS][ROOK], pieces[BLACKS][KNIGHT], pieces[BLACKS][BISHOP], pieces[BLACKS][QUEEN], pieces[BLACKS][KING], pieces[BLACKS][BISHOP], pieces[BLACKS][KNIGHT], pieces[BLACKS][ROOK]},
		{pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN], pieces[BLACKS][PAWN]},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{" ", " ", " ", " ", " ", " ", " ", " "},
		{pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN], pieces[WHITES][PAWN]},
		{pieces[WHITES][ROOK], pieces[WHITES][KNIGHT], pieces[WHITES][BISHOP], pieces[WHITES][QUEEN], pieces[WHITES][KING], pieces[WHITES][BISHOP], pieces[WHITES][KNIGHT], pieces[WHITES][ROOK]}
	};
	memcpy(&grid, &grid2, sizeof(grid));

//	system("/usr/bin/stty raw");
	int mode = menu();
	start(mode);
//	struct winsize w;
//	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//	lcount = w.ws_row;
//	ccount = w.ws_col;
//	printgrid();
//	pthread_t getinput;
//	pthread_create(&getinput, NULL, printinput, NULL);
//	pthread_join(getinput, NULL);
	if (sockfd != 0)
		close(sockfd);
	if (new_fd != 0)
		close(new_fd);
}

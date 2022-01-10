#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#define _BSD_SOURCE

int lcount;
int ccount;
int player_h = 0;
int player_l = 0;
bool **current;
bool **next;
bool firstlaunch = true;
bool computing = false;
int generations = 0;
int sleeptime = 100000;

void calculate()
{
	for (int i = 0; i < lcount; i++)
	{
		for (int j = 0; j < ccount; j++)
		{
			int sum_around = 0;
			int lborder = 0;
			int rborder = 0;
			int tborder = 0;
			int bborder = 0;
			if (j == ccount - 1)
				rborder = 1;
			if (j == 0)
				lborder = 1;
			if (i == lcount - 1)
				tborder = 1;
			if (i == 0)
				bborder = 1;
			for (int k = bborder - 1; k <= 1 - tborder; k++)
			{
				for (int l = lborder - 1; l <= 1 - rborder; l++)
				{
					if (current[i + k][j + l] && (k != 0 || l != 0))
					{
						sum_around += 1;
					}
				}
			}
			if (current[i][j])
			{
				if (sum_around == 2 || sum_around == 3)
					next[i][j] = true;
				else
					next[i][j] = false;
			}
			else
			{
				if (sum_around == 3)
					next[i][j] = true;
				else
					next[i][j] = false;
			}
		}
	}
	for (int i = 0; i < lcount; i++)
	{
		for (int j = 0; j < ccount; j++)
		{
			current[i][j] = next[i][j];
		}
	}
	generations += 1;
}

void clearscreen()
{
	printf("\033[1;1H\033[2J");
}

void markcase()
{
	current[player_h][player_l] = !current[player_h][player_l];
}

void printgrid()
{
	fflush(stdout);
	for (int i = 0; i < lcount; i++)
	{
		for (int j = 0; j < ccount; j++)
		{
			if (i == player_h && j == player_l)
			{
				printf("\033[41m \033[0m");
			}
			else if (current[i][j])
			{
				printf("\033[47m \033[0m");
			}
			else
			{
				printf(" ");
			}
		}
		printf("\r\033[A");
	}
	printf("\r\033[A");
	int strlen = 0;
	int gen_len = generations;
	while (gen_len > 0)
	{
		strlen++;
		gen_len /= 10;
	}
	printf("\r\033[31;47mGenerations : %d%*s\033[0m", generations, ccount - strlen + 50, "\033[47;32mq : exit\033[47m, \033[47;33mm : switch cell status\033[47m, \033[47;34m+ : faster\033[47m, \033[47;35m- : slower\033[47m, \033[47;36mSpace : play/pause\033[47m");
	for (int i = 0; i < lcount + 1; i++)
	{
		printf("\r\033[B");
	}
}

void *play()
{
	while (true)
	{
		if (computing)
		{
			calculate();
			printgrid();
			usleep(sleeptime);
		}
	}
}

void printinput(void)
{
	int c;
	system("/usr/bin/stty raw");
	while ((c = getchar()) != 'q')
	{
		printf("\r\033[2K");
		switch (c)
		{
		case 'A':
			if (player_h != lcount - 1)
				player_h++;
			break;
		case 'B':
			if (player_h != 0)
				player_h--;
			break;
		case 'C':
			if (player_l != ccount - 1)
				player_l++;
			break;
		case 'D':
			if (player_l != 0)
				player_l--;
			break;
		case ' ':
			if (firstlaunch)
			{
				computing = true;
				pthread_t player;
				pthread_create(&player, NULL, play, NULL);
				firstlaunch = false;
			}
			else
				computing = !computing;
			break;
		case 'm':
			markcase();
			break;
		case '+':
			if (sleeptime > 50000)
				sleeptime -= 50000;
			else if (sleeptime > 10000)
				sleeptime -= 10000;
			else if (sleeptime > 1000)
				sleeptime -= 1000;
			break;
		case '-':
			sleeptime += 50000;
			break;
		default:
			break;
		}
		printgrid();
	}
}

int main()
{
	clearscreen();
	printf("\033[?25l");
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	ccount = w.ws_col;
	lcount = w.ws_row - 1;
	current = (bool **)malloc(sizeof(bool *) * lcount);
	next = (bool **)malloc(sizeof(bool *) * lcount);
	for (int i = 0; i < lcount; i++)
	{
		current[i] = (bool *)malloc(sizeof(bool) * ccount);
		next[i] = (bool *)malloc(sizeof(bool) * ccount);
		for (int j = 0; j < ccount; j++)
		{
			current[i][j] = false;
			next[i][j] = false;
		}
	}
	printgrid();
	printinput();
	for (int i = 0; i < lcount; i++)
	{
		free(current[i]);
		free(next[i]);
	}
	free(current);
	free(next);
	printf("\033[?25h");
}

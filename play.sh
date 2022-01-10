#!/bin/sh
compile() {
gcc jeu_principal/main.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11 jeu_principal/map.h jeu_principal/pathfinder.h
}
compile && echo Done || echo Error
./terminations

#!/bin/sh
compile() {
echo -e "\033[34mBuilding terminations\033[0m"
gcc main.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11 map.h pathfinder.h
}
compile && echo Done || echo Error
./terminations
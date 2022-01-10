#!/bin/sh
compile() {
echo -e "\033[34mBuilding chess\033[0m"
gcc chess.c -o chess -lpthread -lncursesw -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding conway\033[0m"
gcc conway.c -o conway -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding minesweeper\033[0m"
gcc minesweeper.c -o minesweeper -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding terminations\033[0m"
#gcc terminations.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11
gcc main.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11 map.h pathfinder.h
}
compile && echo Done || echo Error
./terminations
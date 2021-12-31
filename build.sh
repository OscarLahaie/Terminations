#!/bin/sh
compile() {
echo -e "\033[32mBuilding 2048\033[0m"
gcc 2048.c -o 2048 -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[32mBuilding chess\033[0m"
gcc chess.c -o chess -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[32mBuilding conway\033[0m"
gcc conway.c -o conway -lpthread -Wall -Werror -pedantic -std=c11 -D _DEFAULT_SOURCE
echo -e "\033[32mBuilding minesweeper\033[0m"
gcc minesweeper.c -o minesweeper -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[32mBuilding pacman\033[0m"
gcc pacman.c -o pacman -lpthread -Wall -Werror -pedantic -std=c11 -D _DEFAULT_SOURCE
echo -e "\033[32mBuilding terminations\033[0m"
gcc terminations.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11
}
compile && echo Done || echo Error
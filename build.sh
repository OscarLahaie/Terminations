#!/bin/sh
export OUT_DIR=publish
compile() {
echo -e "\033[34mBuilding chess\033[0m"
gcc mini_jeux/chess.c -o $OUT_DIR/chess -lpthread -lncursesw -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding conway\033[0m"
gcc mini_jeux/conway.c -o $OUT_DIR/conway -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding minesweeper\033[0m"
gcc mini_jeux/minesweeper.c -o $OUT_DIR/minesweeper -lpthread -Wall -Werror -pedantic -std=c11
echo -e "\033[34mBuilding terminations\033[0m"
gcc jeu_principal/main.c -o $OUT_DIR/terminations -lncurses -Wall -Werror -pedantic -std=c11 jeu_principal/map.h jeu_principal/pathfinder.h
}
compile && echo Done || echo Error

[![CI](https://github.com/OscarLahaie/Terminations/actions/workflows/main.yml/badge.svg)](https://github.com/OscarLahaie/Terminations/actions/workflows/main.yml)

# Terminations (nom provisoire)
Projet n°3 d'info de la classe MPI2D de Carnot Dijon
Groupe de Cassandre LAMATY <[@CassandreLAMATY](https://github.com/CassandreLAMATY)>, Oscar LAHAIE <[@OscarLahaie](https://github.com/OscarLahaie)> et Valentin FOULON <[@srgoti](https://github.com/srgoti)>
On nous a dit de faire un jeu, on en a fait [14]

## À propos
Le jeu principal est un jeu du style de Clash Royale (si j'ai bien compris) mais avec des fonctionnalités avancées. Les combats ne seront pas gérés automatiquement, ils pourront lancer des mini-jeux de manière aléatoire

## Les jeux
Les mini-jeux comprennent :
- Pac-Man
- Le démineur (<3)
- 2048
- Les échecs
- La légende raconte que le jeu de la vie de Conway se cache dans la carte du jeu principal

## Comment compiler
```bash
#!/bin/bash
for file in *.c; do gcc -o $(basename $file .c) $file -lpthread -std=c11 -Wall -Wextra -pedantic -D _DEFAULT_SOURCE; done
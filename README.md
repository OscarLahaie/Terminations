[![CI](https://github.com/OscarLahaie/Terminations/actions/workflows/main.yml/badge.svg)](https://github.com/OscarLahaie/Terminations/actions/workflows/main.yml)

# Terminations
Projet n°3 d'informatique de la classe de MP2I du lycée Carnot, Dijon.

#### Groupe composé de :
- LAMATY Cassandre <[@CassandreLAMATY](https://github.com/CassandreLAMATY)> 
- LAHAIE Oscar <[@OscarLahaie](https://github.com/OscarLahaie)>
- FOULON Valentin <[@srgoti](https://github.com/srgoti)>

## À propos
Il s'agit d'un jeu dans le style de Clash Royale / Civilisation mais remanié de façon à être jouable dans le terminal et avec des fonctionnalités simplifiées.
En prime, plusieurs mini-jeux développés par [@srgoti](https://github.com/srgoti) sont disponibles.

#### Les mini-jeux comprennent :
De FOULON Valentin <[@srgoti](https://github.com/srgoti)>
- Le démineur
- Les échecs
- Le jeu de la vie

## Comment compiler
```bash
#!/bin/bash
gcc main.c -o terminations -lncurses -Wall -Werror -pedantic -std=c11 map.h pathfinder.h
./terminations
```
Ou pour avoir le résultat instantanément  
```bash
./build.sh
```

## Contrôles

- p pour play et skip le paramétrage au début du jeu (facilité permettant d'aller plus rapidement au lancement du jeu) ;
- flèches directionnelles pour déplacer le curseur ;
- m pour sélectionner un pion, le déplacer à l'aide des flèches et espace pour valider la case d'arrivée ;
- t pour passer un tour ;


## Compte rendu
### Origine du projet
L'objectif de ce projet était de créer un jeu en C jouable dans le terminal (donc sans interface graphique) mettant en place une utilisation poussée des tableaux.

C'était ainsi pour nous l'occasion de s'essayer à la génération procédurale à l'aide de ceux-ci. De fil en aiguille, le groupe s'est mis d'accord autour de la volontée de créer un jeu de type Clash Royal / Civilization, avec une inspiration graphique venant tout droit du jeu Dwarf fortress.

<br/>

### Fichiers principaux

Terminations comporte 3 fichiers principaux :
- main.c
- map.h
- pathfinder.h
  
#### main.c
Il s'agit du fichiers coordonnant le jeu entier.
C'est ici que nous appelons toutes les fonctions et que nous attribuons les valeurs utilisées.

Ce fichier contient par exemple l'affichage du HUD (design par [@CassandreLAMATY](https://github.com/CassandreLAMATY)), et le menu mis en place par [@OscarLahaie](https://github.com/OscarLahaie) permettant plusieurs paramétrages, notamment au niveau de la taille de la map et de son biome (3 biomes étant disponibles : la plaine (biome par défaut), la tundra et le désert ; chacun des biome apporte diverses fonctionnalités d'affichage pour permettre à l'utilisateur de varier son expérience de jeu).

#### map.h
Fichier header (= fichier contenant les entêtes / fonctions principales du fichier main.c afin d'en limiter la longueur et d'en accroitre la lisibilité.) contenant toutes les fonctions utiles à l'affichage de la map, donc de la génération procédurale de chaque biome, l'affichage des châteaux, des cases d'évènements etc... . Pour résumer, il contient toute la partie graphique brute du jeu hormis le HUD et le menuing.

#### pathfinder.h
Ficher header permettant de gérer les déplacement des pions et de l'IA à travers la map en esquivant les obstacles croisables en cours de route.

<br/>

## Conclusion

Nous avons donc pu créer un jeu permettant l'affrontement entre deux équipes (celle du joueur et l'IA) et la défense des châteaux respectifs tout en mettant en place une génération procédurale sur plusieurs biomes, comprenant par exemple la génération de montagnes, de forêts et de lacs faisant obstacle au déplacement des pions. 

Les difficultés résidaient majoritairement dans la gestion des tableaux en eux mêmes, et parfois dans le fait de déceler des erreurs qui ne sont pas forcément loggées correctement par le terminal lors de crash. Ce projet nous a permis d'approfondir nos connaissances sur les tableaux, notamment avec le pathfinder.

De plus, nous avons pu de nouveau utiliser GitHub pour ce projet, nous permettant ainsi d'améliorer notre utilisation du logiciel et notre aisance vis-à-vis de celui-ci.

<br/>

Des mises à jours seront éventuellement disponibles plus tard afin d'améliorer la qualité du jeu et des actions possibles.

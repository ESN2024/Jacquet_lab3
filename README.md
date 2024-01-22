# ESN11 - Rapport Lab2

## Objectifs
Construire et programmer un système de base Nios II sur un FPGA DE10-Lite pour afficher les coordonnées x, y ou z de l'accéléromètre en liaison I2C. Les coordonnées sont affichés une par une sur 4 chiffres, respectivement sur 4 périphériques 7 segments, et nous pourrons passer d'une coordonnées à l'autre en cliquant sur un bouton poussoir.

## Introduction
Nous allons ainsi voir chaque étape de la réalisation de ce projet, les difficultés, les points clés, les raisons qui nous ont poussés aux différents choix d'implémentation spécifique de notre système.

## Réalisation du système
Nous allons dans un premier construire une représentation du système finale avant de passer à l'étape de la réalisation.

### Schéma bloc fonctionnel

<img width="437" alt="image" src="https://github.com/ESN2024/Jacquet_lab3/assets/127327962/fd569cf4-8440-4c9c-bfce-a6793141c535">


Ce schéma présente l'architecture finale de notre système, la liaison entre notre pc et le système cible, ici la DE10 lite, via un câble USB Blaster. Ici nous visualisons uniquement les périphériques décris dans nos objectifs, évidemment la clock n'apparaît pas, ainsi que le signal de reset présent sur un second bouton poussoire.
L'architecture comporte ainsi différents modules important : 
1. RAM : La mémoire avec une capacité de 40 Mo, amplement suffisant pour cette implémentation.
2. 6 blocs PIO (Parrallel input/output) permettant au FPGA de communiquer avec l'extérieur en lisant des signaux d'entrées et en envoyant des signaux de sortie.
3. Processeur Nios II : Processeur soft-core configurable du FPGA présent sur la DE10 lite. En tant que soft-core, il est implémenté en logique programmable plutôt que fabriqué en tant que puce physique, Ce qui lui donne une nature configurable et nous permet de personnaliser le processeur en fonction de nos besoins spécifiques, réel gain de performances. Ici nous en avons 5 pour piloter les 7 segments, ils sont de 4 bits, ainsi qu'un 6ème de 1 bit pour l'interruption lié au bouton poussoire.
4. Un module timer d'intervalle 1 seconde afin de pouvoir piloter commme voulu notre accéleromètre.
5. Des blocs VHDL afin de convertir nos données sur l'afficheur 7 segments.

### Création du système sous Platform Designer
Après avoir crée sous Quartus 18.1 notre projet nous allons pouvoir avec l'outil Platform Designer crée notre système en déclarant l'ensemble des modules nécessaires ainsi que leurs configurations respectives.
<img width="404" alt="image" src="https://github.com/ESN2024/Jacquet_lab3/assets/127327962/3a15ec49-a7cc-4706-bc67-405d3a103e5a">


Il faudra relier minutieusement ces différents modules pour les interconnecter de façon intelligente.

Lorsque ces étapes ont été réalisé et que Platform Designer ne renvois aucune erreur nous pouvons généré le VHDL correspondant à ce système en l'exportant (Generate/Generate HDL/Selectionner VHDL).

### Création d'un module VHDL BCD7SEG

Nous allons réaliser la description VHDL d'un composant qui permet d'afficher un mot de 4 bits sur un afficheur 7 segments. Ce composant consiste à transcrire un nombre compris entre 0 et 9 en un mot sur 7 bits correspondant à l'état haut ou bas de nos différents segments. Il faudra aussi inclure la fonction d'afficher un signe "-" pour les nombres négatifs sur le 5ème 7 segments.

### Création du Top Level

Ce fichier VHDL consiste à réaliser un liant entre notre Qsys correspondant à nos modules instanciés ainsi que notre composants BCD7SEG, il relie donc ces deux composants à l'aide de signaux sur un Port Map décris en ligne de code.

### Intégration du design généré
Nous allons maintenant revenir sous quartus et réalisé une étape fondamental qui consiste à relié nos broches à nos PIO. Cette étape est réalisé avec l'outil Pin Planner de Quartus (Assignments/Pin Planner). Avant cela il faudra réalisé le check Analysis & Synthesis pour vérifier notre développement jusque là et intégré l'assignation des broches. Une fois cette étape réalisé nous allons pouvoir lancé l'outil Pin Planner et assigné nos broches à nos PIO, attention il faut bien vérifié que le device est le même que le système cible auquel cas nos broches seront différentes (Quartus/Assignments/Device) ici nous développons sur la MAX 10 - 10M50DAF484C7G :

<img width="365" alt="image" src="https://github.com/ESN2024/Jacquet_Lab1/assets/127327962/38226d07-c961-448a-9e1f-cefcc015430f">

Nous retrouvons donc une visualisations de nos broches, avec en dessous nos différents signaux PIO à relier :

<img width="197" alt="image" src="https://github.com/ESN2024/Jacquet_lab3/assets/127327962/d0b5b5e1-ea86-40b2-a676-9a97cc824e31">

Une fois l'intégralité de nos signaux récupérés, nous pouvons fermé Pin Planner, revenir sur Quartus et lancer le Compile Design afin de compiler l'entiereté de nos système et récupérer les fichiers nécessaires à la suite du développement de notre projet.

### Génération de la BSP
Nous allons maintenant passer à une étape clé, la génération de notre BSP (Board Support Package), facilite la communication entre le logiciel et le matériel, cette étape se réalise sous le terminal Nios II avec la commande : 
<img width="409" alt="image" src="https://github.com/ESN2024/Jacquet_Lab1/assets/127327962/2daa0ab4-defb-4bd4-bf3d-6370fa76a2e8">

Ce qui nous génére un fichier que nous stockerons dans le répertoire /soft, à coté de /app où seront stockés notre code .c et la makefile associé.

### Création du code source

Afin de piloter nos périphériques de façon à créer un chenillard de leds, lancé par un bouton poussoir et à vitesse réglable par des switchs, nous allons écrire un code en C, voici les étapes :

1. Génération des bibliothèques
   #include <stdio.h>: Bibliothèque standard C pour les entrées/sorties, comme printf et scanf.
   #include "system.h": Fichier de configuration spécifique au système Nios II, définit la configuration matérielle.
   #include "sys/alt_sys_init.h": Initialise les composants système d'Altera pour Nios II.
   #include <io.h>: Fournit des fonctions d'entrée/sortie bas niveau, souvent utilisées pour les opérations sur les registres.
   #include <alt_types.h>: Définit des types de données spécifiques à Altera, comme alt_u32 pour les entiers non signés 32 bits.
   #include <sys/alt_irq.h>: Gère les fonctions liées aux interruptions dans les systèmes Nios II d'Altera.
   #include "altera_avalon_pio_regs.h": Fournit des définitions et des fonctions pour manipuler les registres des blocs PIO d'Altera.
   #include "altera_avalon_timer_regs.h":Fournit les fonctions liés au timer.


2. Fonction READ_DATA
   Viens lire écrire sur les registres liés aux valeurs de l'accéleromètre en x0, x1, x2, y0, y1, y2 avec des fonctions tel que I2C_Start, I2C_Write et I2C_Read.

3. Fonction d'affichage sur un 7 segments
   Cette fonction viens décrire nos valeurs dans 4 entiers u, d, c,m respectivement unité, décimal, centaine, millier afin de pouvoir afficher nos valeurs sur 4 périphériques 7 segments.

4. Fonction d'interruption du switch key
   Cette fonction est chargé de pouvoir modifier la valeur affiché sur les 7 segments, que ce soit x, y ou z, en incrémentant un compteur.

5. Fonction de traitement des nombres négatif
   Cette fonction réalise le complément à deux sur une donnée afin d'en extraire sa valeur réelle seulement pour les signaux ayant leur bit de poids fort à l'état haut. Nous récuperons ensuite une donnée indiquant si le signal était ou non signé afin de pouvoir afficher sur le 5ème 7 segments le signe.

6. Fonction principale Main()
   Cette fonction n'est pas très bien réalisé, elle pourrait être simplifié avec plus de fonctions mais par soucis de temps cela n'a pas pu être fait. Elle se charge d'initialiser la liaison I2C, initialise les interruptions ainsi que les adresse des registres du device et des données de l'accéleromètre. Elle concatène les valeurs des bits poids fort et faible de chaque axes x, y ou z en réalisant un OU avec décalage de 8 bits. Elle possède aussi la "machine à état" d'affichage des axes, c'est à dire que dans l'état 0 nous affichons X, à l'état 1, y et etc...


## Déploiement du projet sur le système cible

Nous revenons à notre terminale Nios II, où nous avons précédemment généré notre BSP, nous allons maintenant généré automatiquement un Makefile avec la commande :
   <img width="295" alt="image" src="https://github.com/ESN2024/Jacquet_Lab1/assets/127327962/09d00f23-ebc5-48b9-9e92-41651effd3f8">

Ce Makefile nous permettra de compiler notre code C ainsi que l'intégralité de notre projet afin de le tester sur notre DE10 lite. une fois généré il nous suffit d'utiliser la commande make à l'endroit où a été généré le Makefile c'est à dire aux cotés du code C. Si aucune erreur n'est remonté, le programme est prêt à être lancé sur la DE10, attention à bien vérifié la connection à la carte avec l'USB Blaster (Tools/Programmer/Start). C'est à l'aide de cette commande : 
<img width="216" alt="image" src="https://github.com/ESN2024/Jacquet_Lab1/assets/127327962/ede777e3-89ba-438c-a72b-e3599f1dcdbb">

que nous allons déployer le programme, et à l'aide de Nios2-terminale.exe suivre les retours renvoyé par nos alt_printf sur notre terminale.

Voici la démonstration finale de notre programme en vidéo : 
(https://github.com/ESN2024/Jacquet_lab2/blob/main/IMG_3173.MOV)

## Conclusion

Pour conclure ce dernier projet d'ESN11 nous a permis d'aller encore plus loin dans notre compréhension du codesign, en mettant en place une liaison I2C pour l'accéleromètre. L'une des premières difficultés aura été la compréhension des documents et datasheet permettant l'implantation de ce périphérique en I2C, énormement d'infos difficile à comprendre et mettre en place. Suivi du rodage du périphérique pour instancier les valeurs des axes avec les bonnes valeurs d'offset, où une méthode dites de dichotomie aura été nécessaire, malgrè l'aspect simpliste de la méthode elle nous aura permis de réaliser cette étape. Malheureusement ce projet n'aura pas pu être terminé, actuellement je suis capable d'afficher l'un des 3 axes sur les 7 segments avec le bon signe devant, malheuresement les valeurs ne sont pas parfaite, et l'initialisation par dichotomie semble avoir eu un problème et ne fonctionne plus aussi bien, je n'ai pas eu le temps de réaliser une seconde fois cette méthode. 

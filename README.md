# M1 IMAGINE Projet Image

[FR]

Ce projet s'inscrit dans le contexte de la matière traitement d'images en M1 IMAGINE. C'est un projet réalisé en binôme aléatoire, il a été réalisé en c++ et l'interface a été réalisé avec QT.

Le but : réaliser un générateur de cartes aléatoires

Nous avons généré diverses cartes avant d'obtenir notre carte finale. Nous avons tout d'abord généré une carte de hauteur afin de définir les reliefs (que ce soit au niveau de la terre ou de la mer), pour cela nous avons utilisé du bruit de Perlin. Ensuite, nous avons voulu ajouter des rivières et des lacs, nous avons réalisé une carte de gradient puis une carte de rivières. Cette carte de rivière a été crée en prenant des points aléatoires sur la carte de hauteur et en suivant le gradient depuis ces points. Nous avons aussi voulu ajouter différents climats en générant une carte de climat. Cette carte a été générée avec du bruit de Perlin avec des couleurs rouges (pour la température) et vertes (pour l'humidité). Nous avons associé chaque zone de cette carte à un climat différent (si on a une valeur de température basse et une humidité haute, on aura une zone enneigée en blanc par exemple). Enfin, nous avons aussi ajouté des points d'intérêts à la carte. Pour cela, on a déterminé les zones importantes de l'image (lacs, forêts...) et on a généré un nom aléatoire pour ces zones.

***

[EN]

This project is part of the image processing course in the M1 IMAGINE program. It is a project carried out in random pairs, developed in C++, with the interface created using QT.

The goal : to create a random map generator

We generated various maps before having our final map. At the beginning, we created a height map to define the terrain features (both land and sea) using Perlin noise. Next we aimed to add rivers and lakes, so we generated a gradient map then a river map. This river map has been created by taking random points on the height map and following the gradient from those points. We also wanted to add different climates by generating a climate map. This map was generated using Perlin noise with red colors (for temperature) and green colors (for humidity). Each zone on this map was associated with a different climate (for instance, if a zone has low temperature and high humidity, it will be a snowy area in white). Finally, we added points of interest to the map. To do this, we determined the important areas of the image (lakes, forests, etc.) and generated a random name for these areas.

https://github.com/LeaSerrano/M1-IMAGINE-ProjetImage/assets/113998552/940b6006-858d-440f-acfc-fbb556d6b4cb

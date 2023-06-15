Projet C++,
NOEL Océan ROB24,
04/01/2022.

////////////////////
////PRESENTATION////
////////////////////

Ce projet génère une image binaire en couleur des fonds marins à partir d'un fichier de relevés.
La structure des relevés doit être similaire à celle des fichiers .txt donnés avec ce projet dans le dossier "assets/".
Les caractéristiques du programme sont les suivantes :
- L'image est générée en binaire.
- L'image est généré en couleur.
- Le projet est compilé avec Cmake.
- La projection utilisée est celle de LAMBERT93.
- Les entrées du programme sont le chemin vers le fichier de données et la largeur de l'image à générée.
- Les contours du MNT sont proprement rendus (même pour des enveloppes non convexes).
- Une documentation HTML à été réalisée et est accessible via le fichier html/index.html.
- Un algorithme d'optimisation pour la recherche des triangles à été implémenté.
- L'image est générée avec des ombres

Le repertoire du projet est organisé de la manière suivante : 
- assets : Fichiers utilisés ou générés par le projet. 
(les relevés doivent être placés ici et l'image généré sera aussi placée dan sce repertoire)
- html : Documentation HTML du projet. (Visualisable en lancant index.html)
- include : Librairies ou fichier extérieurs au projet. (pas besoin de les installer)
- src : Codes sources.

////////////////////
////DEPENDANCES/////
////////////////////

Les librairies suivantes sont à installer pour que le projet fonctionne :
*proj.h :
	"sudo apt-get install proj-dev" ou
	"sudo apt-get install libproj-dev"
*VTK :
	"sudo apt install libvtk7-dev"

Il faut aussi mettre les données du MNT (.txt) dans le dossier "assets/" de la racine du projet.

////////////////////
////QUICK START/////
////////////////////

Avant de commencer il faut mettre les données du MNT dans le dossier "assets/" qui est à la racine du projet.

Des fichiers bash on été créé pour faciliter la compilation et le lancement du programme :

"bash initialize_cmake.sh" : Initialisation et Cmake et création du dossier build.
"bash build.sh" : Compilation du projet.
"bash build_n_launch.sh 'fichier.txt' 'width'" : Compilation et lancement du programme avec les paramètres détaillés ci-dessous.

Détail des arguments :
- fichier.txt --> nom du fichier .txt qui contient les mesures.
- width --> largeur en pixel de l'image à générer.


//////////////////////////////
////COMPILATION AVEC CMAKE////
//////////////////////////////

Pour mettre en place la structure Cmake du projet avant la compilation, et créer le dossier build ou sera l'executabe, 
il faut executer la commande suivante à la racine :

"bash initialize_cmake.sh"

Un dossier build devrait apparaitre.

Ensuite, pour compiler le projet avec Cmake il faut executer la commande suivante à la racine :

"bash build.sh"

///////////////////////////////////////////
////LANCEMENT (si utilisation de CMAKE)////
///////////////////////////////////////////

Pour compiler et lancer le programme, un fichier bash à également été créé, la commande suivante permet de le lancer :

"bash build_n_launch.sh 'fichier.txt' 'width'"

Détail des arguments :
- fichier.txt --> nom du fichier .txt qui contient les mesures.
- width --> largeur en pixel de l'image à générer.

Ce fichier bash réalise plusieurs actions :
1 - Compilation du projet (Un executable est généré dans le dossier "build")
2 - Lancement du programme (une image .ppm est généré dans le dossier "build")
3 - Déplacement de l'image .ppm dans le dossier "assets" de la racine du projet
4 - Ouverture de l'image avec "xdg-open"

Le résultat visuel devrait apparaitre si xdg est installé sur votre PC.
Autrement, vous pouvez ouvrir l'image manuellement dans le dossier "assets/" à la racine du projet.


Sinon, une fois l'executable créé dans le dossier build, il est possible de le lancer via cette commande à éxécuter dans "build" :

"./create_raster 'fichier.txt' 'width'" avec les même arguments détaillés ci-dessus.

Dans ce cas, l'image générée se trouvera dans le dossier 'build'.


///////////////////////////////////////////
////COMPILATION ET LANCEMENT SANS CMAKE////
///////////////////////////////////////////

Il possible de compiler et lancer le programme sans Cmake, pour cela voilà les étapes :
1 - Compiler le fichier "src/main.cpp" avec le compilateur de votre choix.
2 - Garder l'executable dans le dossier "src" et le lancer avec 2 arguments : 
"Nom du fichier .txt contenant les relevés" et "Largeur de l'image à générer en pixels"
3 - Ouvrir l'image .ppm qui à été générée dans le dossier "src"





#include <cstdlib> // bibliothèque générique standard
#include <iostream> // bibliothèque d’entrées/sorties
#include <string>
#include <unistd.h>
#include <ctime> //temps, mesures d'executions
#include <vector> //vecteur
#include <map> //dictionnaires
#include "triangulation.h" //fonctions pour triangulation
#include "struct_point.h" //définissions de la structure d'un points
#include "init_points_pixels.h" //fonctions pour initialisation des poinst et pixels
#include "generate_image.h" 

using namespace std;  

/**
* \file main.cpp
* \brief Fichier principale qui organise et lance les différentes fonctions. 
* Il génère un rendu du MNT passer en arguments de dimensions données en arguments également.
* Le rendu généré est une image binaire en couleurs qui elle utilise la colormap Haxby.
* \date 04/01/2022
* \author NOEL Océan
*/

int main(int argc, char *argv[])
{
	////////////////////
	////INITIALISATION//
	////////////////////

	//variables analytiques
	time_t t0,tf; 
	time(&t0);

	//initialisation des variables
	map<string,double> context; //variable qui permet de stocker des paramètre important du programme (évite les variables globales)
	vector<point> points;  //stocke les points de relevés de mesures
	vector<double> points_line;  //stocke les points sous forme {x0,y0,x1,y1} (utilisé lors de la triangulation)
	vector<int> pixels; //stock la valeure des pixels sous forme {pixel1,pixel2,...}
	vector<double> pixels_illumination; //stock la valeure de l'illumination des pixels sous forme {ombre_pixel1,ombre_pixel2,...} (0 = non illuminé, 1 = illuminé au maximum)
	string file_name; //nom du fichier à ouvrir pour les valeurs 
	int image_size; //largeur en pixel de l'image à générer
	vector<double> sun_dir = {-1,0,0}; //direction de la lumière du soleil
	context["sun_dir_x"] = sun_dir[0];
	context["sun_dir_y"] = sun_dir[1];
	context["sun_dir_z"] = sun_dir[2];
	context["default_color"] = 0; //couleur par défaut des pixels
	context["nb_colors"] = 100000; //nombre de couleurs dans la colormap possibles pour les pixels (échantillonage)
	context["max_x"] = -99999999; //initialisée dans "project_points()"  : ordonnées x maximale des points
	context["min_x"] = 99999999; //initialisée dans "project_points()"  : ordonnées x minimale des points
	context["max_y"] = -99999999; //initialisée dans "project_points()"  : ordonnées y maximale des points
	context["min_y"] = 99999999; //initialisée dans "project_points()"  : ordonnées y minimale des points
	context["min_depth"] = -99999999; //initialisée dans "project_points()"  : profondeur minimale des points
	context["max_depth"] = 99999999; //initialisée dans "project_points()"  : profondeure maximale des points
	context["lg_pix"] = 0; //initialisée dans "create_pixels()"  : longeur d'un pixel en m
	context["h_pix"] = 0; //initialisée dans "create_pixels()"  : hauteur d'un pixel en m

	//lecture et initialisation des arguments
	if (argc==3){
		file_name = argv[1];
		image_size = stod(argv[2]);
	}
	else{
		cout << "Arguments incorrects, il faut (uniquement) : "<<endl;
		cout << "- le chemin/nom du fichier de donnees"<<endl;
		cout << "- la largeur de l’image generee, en pixels" <<endl;
		return 0;
	}
	context["width"] = image_size; //largeur de l'image en pixels
	context["height"] = image_size; //hauteur de l'image en pixels

	cout <<endl<< "Starting program with arguments : [" <<file_name<<","<<image_size<<"]"<<endl;

	/////////////////////////////////
	/////ACQUISITION ET ANALYSE//////
	/////////////////////////////////

	//création et récupération des points en coordonnées géographiques
	cout <<endl<< "Data initialisation :" <<endl;
	int result = get_points("../assets/"+file_name,&points); //Voir init_point_pixels.cpp)
	if(result == 0){
		cout << "echec de la récupération des points" << endl;
		return 0;
	}

	//projection des points et calculs de plusieurs paramètres du nuage de points
	project_points(&points,points_line,context); //(Voir init_point_pixels.cpp)

	//Création et intialisation des pixels 
	create_pixels(pixels,pixels_illumination,context); //(Voir init_point_pixels.cpp)

	/*cout << "Context : "<< context["default_color"] << " , "<<
	context["nb_colors"]  << " , "<<
	context["max_x"]  << " , "<<
	context["min_x"]  << " , "<<
	context["max_y"]  << " , "<<
	context["min_y"]  << " , "<<
	context["min_depth"] << " , "<<
	context["max_depth"] << " , "<<
	context["lg_pix"]  << " , " <<
	context["h_pix"] << endl;*/

	//////////////////
	/////CALCULS//////
	//////////////////
	
	//Triangulation et coloration
	triangulate_n_color(points,points_line,pixels,pixels_illumination,context); //(Voir triangulation.cpp)

	////////////////////
	/////AFFICHAGE//////
	////////////////////

	result = generate_image(pixels,pixels_illumination,context); //generation de l'image binaire colorée
	if (result == 0){
		cout << "echec d'ouverture du fichier image" << endl;
		return 0;
	}

	cout<<endl<<"END"<<endl;

	time(&tf);
	cout<<endl<<"Temps total d'execution : ("<<tf-t0<<" s)"<<endl<<endl; //affichage du temps d'éxecution

}
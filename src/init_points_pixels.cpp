#include <cstdlib> // bibliothèque générique standard
#include <iostream> // bibliothèque d’entrées/sorties
#include <string>
#include <unistd.h>
#include <fstream> //manipulation fichiers
#include <iomanip> //pour conversion string en stream
#include <vector> //vecteur
#include <map> //dictionnaires
#include <proj.h> //projection
#include <math.h>
#include "struct_point.h"
#include "init_points_pixels.h"

/**
* \file init_points_pixels.cpp
* \brief Fichier d'implémentation des fonctions utilisée pour initialiser les points et pixels.
* \date 04/01/2022
* \author NOEL Océan
*/

using namespace std;

void create_pixels(vector<int> &pixels, vector<double> &pixels_illumination, map<string,double> &context){
	/**
	* \brief Cette fonction créer les pixels qui vont quadriller le nuage de points donner en paramètre. 
	* De plus elle ajoute à la variable context les infos importantes telles que la largeur et la hauteur en m d'un pixel.
	* \param pixels	Vecteur dans lequel initialiser la couleur de nos pixels.
	* \param pixels_illumination Vecteur dans lequel intialiser les illuminations des pixels (0 = non illuminé, 1 = illuminé au maximum).
	* \param context Dictionnaire qui contient les données importantes (evite les variables globales).
	* Ici cette fonction à besoin de :
	* - valeurs limites des positions des points (min_x,min_y,max_x,max_y)
	* - nombre de pixels voulus (width,height)
	* - couleur par défaut des pixels (default_color)
	*/

	////////////////////
	////INITIALISATION//
	////////////////////

	//Récupération des variables nécessaires au calcul du quadrillage
	double width = context["width"];
	double height = context["height"];
	double max_x = context["max_x"];
	double min_x = context["min_x"];
	double max_y = context["max_y"];
	double min_y = context["min_y"];
	double default_color = context["default_color"];

	//variables analytiques
	time_t t0,tf; 
	double compteur = 0; 
	int progress = 0; 

	//Calcul des hauteurs et largeur des pixels, et mise à jour de la variable context
	float lg_pix = (max_x-min_x)/width; //largeur d'un pixel en m
	float h_pix = (max_y-min_y)/height; //hauteur d'un pixel en m
	context["lg_pix"] = lg_pix;
	context["h_pix"] = h_pix;

	////////////////
	////OPERATIONS//
	////////////////

	//création des pixels et initialisation
	cout << "- Creating pixels...";
	time(&t0);
	compteur = 0;
	progress = 0;

	for (int i = 0; i < width*height; i++) {
		pixels.push_back(default_color); //un pixel est représenté par un "int" qui correspond à un indice de couleur
		pixels_illumination.push_back(1); //illumination maximale initialement pour les pixels

		//Affichage de la progression
		compteur +=1;
		progress = 100*compteur/(width*height);
		string progress_str = to_string(progress);
		cout << progress_str << " %";
		cout << "\b\b";
		for(int i=0; i<progress_str.size();i++){
			cout << "\b";
		}
	}
	//cout << pixels.size();

	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution
}

void project_points(vector<point> *v, vector<double> &points_line, map<string,double> &context)
{
	/**
	* \brief Cette fonction transforme les coordonnées géographique d'une liste de point en coordonnées planaire, 
	* c'est une projection Lambert93. De plus elle initialise plusieurs paramètres important de "context" telle que les limites 
	* du nuage de points.
	* \param v Vecteur dans lequel se trouve les points en coordonnées géographique.
	* \param points_line Vecteur dans lequel se trouve les points en coordonnées géographique sous forme {x0,y0,x1,y1...}.
	* \param context Dictionnaire qui contient les données importantes (evite les variables globales).
	* Ici cette fonction en à besoin pour initialiser certaines des valeurs.
	*/

	////////////////////
	////INITIALISATION//
	////////////////////

	//variables analytiques
	time_t t0,tf; 
	double compteur = 0; 
	int progress = 0; 
	cout << "- Projecting points...";

	PJ_CONTEXT *C;
	PJ *P; //fonction de transformation/projection
	PJ_COORD a, b; //coordonnés géographique et coordonnées projeté
	
	//Création de la fonction de projection
	C = proj_context_create();
	P = proj_create_crs_to_crs(C, "+proj=longlat +datum=WGS84","+proj=lcc +lat_1=49 +lat_2=44 +lat_0=48.199161330566646 +lon_0=-3.0146392003209987 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs",NULL);

	if (0 == P) {
	    fprintf(stderr, "Failed to create transformation object.\n");
	}

	////////////////
	////OPERATIONS//
	////////////////

	time(&t0);
	int nb_ops = v->size();
	//projection

	double min_x = context["min_x"];
	double max_x = context["max_x"];
	double min_y = context["min_y"];
	double max_y = context["max_y"];
	double min_depth = context["min_depth"];
	double max_depth = context["max_depth"];
	for(auto it = v->begin(); it != v->end(); ++it) //iteration sur tout les points
	{
		a = proj_coord((*it).x,(*it).y, 0, 0); //coordonnée terrestres à convertir (longitude, latitude)
		b = proj_trans(P, PJ_FWD, a); //coordonnée projetée
		(*it).x = b.xy.x;
		(*it).y = b.xy.y;

		//calcul des limites du nuage de point
		min_x = min(min_x,(*it).x);
		max_x = max(max_x,(*it).x);
		min_y = min(min_y,(*it).y);
		max_y = max(max_y,(*it).y);
		min_depth = max(min_depth,(*it).depth); //inversion min, max car profondeur négative
		max_depth = min(max_depth,(*it).depth);

		//initialisation des autres structures de stockages des points
		double x = (*it).x;
		double y = (*it).y;
		points_line.push_back(x);
		points_line.push_back(y);

		//Affichage de la progression
		compteur +=1;
		progress = 100*compteur/(nb_ops);
		string progress_str = to_string(progress);
		cout << progress_str << " %";
		cout << "\b\b";
		for(int i=0; i<progress_str.size();i++){
			cout << "\b";
		}

	}
	context["min_x"] = min_x;
	context["max_x"] = max_x;
	context["min_y"] = min_y;
	context["max_y"] = max_y;
	context["min_depth"] = min_depth; //inversion min, max car profondeur négative
	context["max_depth"] = max_depth;


	//cout << "End of projection." << endl;
	//cout << "min_x = " << min_x << " , max_x = " << max_x <<" , min_y = " << min_y<<" , max_y = " << max_y <<" , min_depth = " << min_depth << " , max_depth = " << max_depth <<endl;
	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution

}

int get_points(string file_name, vector<point> *v)
{
	/**
	* \brief Lis une liste de données de points au format .txt et les stock sous forme de vecteur de point.
	* \param file_name nom du fichier dans lequel se trouve les données textuelles.
	* \param v vecteur dans lequel sauvegarder les points récupérés.
	*/

	//variables analytiques
	time_t t0,tf; 
	time(&t0);

	cout<<"- Getting points from file...";
	ifstream f(file_name); //tentative d'ouverture du fichier
	if (!f.is_open())
	{
		cout << "Echec d'ouverture de " << file_name << endl;
		return 0;
	}

	//Initilisation des variables
	string str;
	point p1;

	//parcours des lignes du fichier
	while(!f.eof())
	{
		getline(f,str);
		//conversion des données textuelles en point.
		get_point(p1,str);
		//ajout du point au vecteur
		v->push_back(p1);
	}

	f.close();
	//cout << v->size() <<" points got."<<endl;
	
	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution

	return 1;

}

void get_point(point& p,string& str)
{
	/**
	* \brief Stocke les données txt d'un point dans une structure point.
	* \param p point dans lequel sauvegarder les données.
	* \param str données textuelles du point.
	*/
	string x,y,depth;
	std::istringstream line_stream(str);
	line_stream >> y >> x >> depth; //déserialisation
	//cout << str << endl;
	//cout << std::setprecision(14) << x << " , " << y << " , " << depth << endl;
	p.x = stod(x); //enregistrement
	p.y = stod(y);
	p.depth = stod(depth);
	if(p.depth > 0){ //si la profondeur est exprimmée positivement, on change son signe pour fonctionner avec des profondeurs négatives
		p.depth = -p.depth;
	}
	//cout << std::setprecision(14) << p.x << "," << p.y << "," << p.depth << endl; 
}
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include "delaunator.hpp"
#include "Triangle.h"
#include "triangulation.h"
#include "struct_point.h"

using namespace std;

/**
* \file triangulation.cpp
* \brief Fichier d'implémentation des fonctions utilisées pour la triangulation
* \date 04/01/2022
* \author NOEL Océan
*/

double width;
double height;
double default_color;
double lg_pix;
double h_pix;
double max_x;
double min_x;
double max_y;
double min_y;
int nb_colors;
double min_depth;
double max_depth;

void triangulate_n_color(vector<point> &points, vector<double> &points_line,vector<int> &pixels, vector<double> &pixels_illumination,map<string,double> &context){
	/**
	* \brief Calcul les triangles de delaunay et en déduit une coloration pour les pixels.
	* \param points_line Coordonnées des points en m sous forme {x0,y0,x1,y1...}.
	* \param pixels Liste des pixels.
	* \param pixels_illumination Vecteur des illuminations des pixels.
	* \param context Dictionnaire qui contient les données importantes (evite les variables globales).
	* Ici cette fonction en a besoin pour appeler ses fonction auxilliaires et elle utilise : 
	* - le vecteur lumière qui génère les ombres (sun_dir_x,sun_dir_y,sun_dir_z)
	*/

	//variables analytiques
	time_t t0,tf; 
	cout << endl<<"Triangulation and coloration :"<<endl<<"- Creating Triangles...";
	time(&t0);
	int progress = 0;

	//récupération des variables nécessaires
	width = context["width"];
	height = context["height"];
	default_color = context["default_color"];
	lg_pix = context["lg_pix"];
	h_pix = context["h_pix"];
	max_x = context["max_x"];
	min_x = context["min_x"];
	max_y = context["max_y"];
	min_y = context["min_y"];
	nb_colors = context["nb_colors"];
	min_depth = context["min_depth"];
	max_depth = context["max_depth"];
	double sun_dir_x = context["sun_dir_x"];
	double sun_dir_y = context["sun_dir_y"];
	double sun_dir_z = context["sun_dir_z"];

	//calcul des triangles sous forme {x0,y0,x1,y1,x2,y2}
	delaunator::Delaunator d(points_line);

	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution


	/////////////////////////////////////////////
	////Optimisation pour les formes non convex//
	/////////////////////////////////////////////

	//détermination de la longueur maximale d'un coté de triangle à avoir
	cout << "- Optimizing triangles for non-convex forms...";
	time(&t0);
	size_t nb_triangles = d.triangles.size();
	double lim_triangle_lg = 0;
	double mean = 0;
	double ecar_type = 0; 
	double max_norm = 0;
	double min_norm = 9999999;

	for(std::size_t i = 0; i < nb_triangles; i+=3) {
    	//données de delaunator
    	double x0 = d.coords[2 * d.triangles[i]];
    	double y0 = d.coords[2 * d.triangles[i] + 1]; 
    	double x1 = d.coords[2 * d.triangles[i + 1]];
    	double y1 = d.coords[2 * d.triangles[i + 1] + 1];
    	double x2 = d.coords[2 * d.triangles[i + 2]];
    	double y2 = d.coords[2 * d.triangles[i + 2] + 1]; 

    	//calcul de la taille des segments
    	vector<double> v1 = {x1-x0,y1-y0}; 
		vector<double> v2 = {x2-x1,y2-y1}; 
		vector<double> v3 = {x0-x2,y0-y2}; 
		double norm1 = sqrt(pow(v1[0],2)+pow(v1[1],2));
		double norm2 = sqrt(pow(v2[0],2)+pow(v2[1],2));
		double norm3 = sqrt(pow(v3[0],2)+pow(v3[1],2));

		//on considère uniquement le segment le plus grand
		double norm_to_consider = max({norm1,norm2,norm3});

		//ajout pour futur calcul de la moyenne et ecart_type
		mean += norm_to_consider;
		ecar_type += pow(norm_to_consider,2);
		max_norm = max(max_norm,norm_to_consider);
		min_norm = min(min_norm,norm_to_consider);

    	//Affichage de la progression
		progress = 100*i/(nb_triangles);
		string progress_str = to_string(progress);
		cout << progress_str << " %";
		cout << "\b\b";
		for(int i=0; i<progress_str.size();i++){
			cout << "\b";
		}

    }

    //calcul de la moyenne et de l'écart-type des plus grands segments de triangles
    mean = mean/(nb_triangles/3);
    ecar_type = sqrt(ecar_type/(nb_triangles/3)-pow(mean,2));

    //calcul de l'ecart_type relatif à la moyenne
    double relative_std = abs(ecar_type/mean);

    //définition de la longeur maximale pour un coté de triangle
    lim_triangle_lg = min(max_norm+1,(1+(1/relative_std))*mean);

    time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution


	/////////////////////////////////////////////////////
	////Generation des triangles optimisés et coloration//
	/////////////////////////////////////////////////////

	//création des triangles sous forme {p1,p2,p3} et coloration des pixels
	cout << "- Generating new triangles and coloration...";
    time(&t0);
	progress = 0;
    for(std::size_t i = 0; i < nb_triangles; i+=3) {
    	//données de delaunator
    	double x0 = d.coords[2 * d.triangles[i]];
    	double y0 = d.coords[2 * d.triangles[i] + 1]; 
    	double x1 = d.coords[2 * d.triangles[i + 1]];
    	double y1 = d.coords[2 * d.triangles[i + 1] + 1];
    	double x2 = d.coords[2 * d.triangles[i + 2]];
    	double y2 = d.coords[2 * d.triangles[i + 2] + 1]; 

    	//si un des segments du triangle est trop long, on ignore ce triangle,
    	//cela permet d'avoir des contours mieux définit pour des formes non convexes.

    	//calcul de la taille des segments
    	vector<double> v1 = {x1-x0,y1-y0}; 
		vector<double> v2 = {x2-x1,y2-y1}; 
		vector<double> v3 = {x0-x2,y0-y2}; 
		double norm1 = sqrt(pow(v1[0],2)+pow(v1[1],2));
		double norm2 = sqrt(pow(v2[0],2)+pow(v2[1],2));
		double norm3 = sqrt(pow(v3[0],2)+pow(v3[1],2));

		bool too_long = (norm1 > lim_triangle_lg) || (norm2 > lim_triangle_lg) ||(norm3 > lim_triangle_lg);

    	if (!too_long){ //si le triangle est trop grand, on l'ignore et on passe au suivant

    		//points correspondants aux données delaunator

	    	point* p1 = &(points[d.triangles[i]]);
	    	point* p2 = &(points[d.triangles[i + 1]]);
	    	point* p3 = &(points[d.triangles[i + 2]]);

	    	//création du triangle sous forme {p1,p2,p3} (liste de points)
			Triangle T = Triangle(p1,p2,p3);
			//cout << "Triangle points: [["<< T.p1->x<<", "<< T.p1->y<<"], ["<< T.p2->x<<", "<< T.p2->y<<"], ["<< T.p3->x<<", "<< T.p3->y<< "]]"<<endl;

			//calcul de l'illumination de ce triangle
			vector<double> sun_dir = {sun_dir_x,sun_dir_y,sun_dir_z};
			T.compute_illumination(sun_dir);

	    	//récupération des indices des pixels qui sont dans ce triangle et coloration
	    	find_pixels(T,pixels,pixels_illumination);
    	}

    	//Affichage de la progression
		progress = 100*i/(nb_triangles);
		string progress_str = to_string(progress);
		cout << progress_str << " %";
		cout << "\b\b";
		for(int i=0; i<progress_str.size();i++){
			cout << "\b";
		}
	}

	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'éxecution

}


void find_pixels(Triangle &T,vector<int> &pixels,vector<double> &pixels_illumination){
	/**
	* \brief Trouve l'indice des pixels qui appartiennent au triangle T et les colors.
	* \param T Triangle à considérer.
	* \param pixels Liste des pixels.
	* \param pixels_illumination Vecteur des illuminations des pixels.
	* Cette fonction à aussi besoin des variables globales suivantes :
	* - nombre de pixels de l'image (width,height)
	* - couleur par défaut d'un pixel (default_color)
	* - les dimensions d'un pixel (lg_pix,h_pix)
	* - les limites minimales de l'image (min_x,min_y,max_y)
	*/

	////////////
	////ETAPE1//
	////////////

	//ETAPE 1: Calculer le plus petit rectangle de pixels contenant les 3 sommets pour réduire le temps de recherche

	//calcul des pixels des 3 sommets
	int pix_p1 = pixel_of_point(*(T.p1));
	int pix_p2 = pixel_of_point(*(T.p2));
	int pix_p3 = pixel_of_point(*(T.p3));
	//cout<< "Pixels : " << "["<<pix_p1<<","<<pix_p2<<","<<pix_p3<<"]"<<endl;
	
	//calcul des coordonnées de ces pixels
	int coy_1,coy_2,coy_3; //pixel numéro ... sur l'axe des ordonnées
	compute_coords_y(pix_p1,coy_1,width);
	compute_coords_y(pix_p2,coy_2,width);
	compute_coords_y(pix_p3,coy_3,width);
	int cox_1 = min(width-((coy_1*width)-pix_p1),height); //pixel numéro ... sur l'axe des abscisses
	int cox_2 = min(width-((coy_2*width)-pix_p2),height);
	int cox_3 = min(width-((coy_3*width)-pix_p3),height);
	//cout << "["<<cox_1 << " , " << coy_1 << "]"<< "["<<cox_2 << " , " << coy_2 << "]"<< "["<<cox_3 << " , " << coy_3 << "]" <<endl;

	//calcul des limite du carrée de pixels
	int min_cox_pix = min({cox_1,cox_2,cox_3});
	int min_coy_pix = min({coy_1,coy_2,coy_3});
	int max_cox_pix = max({cox_1,cox_2,cox_3});
	int max_coy_pix = max({coy_1,coy_2,coy_3});
	//cout <<"["<< min_cox_pix << " , " << max_cox_pix << " , " << min_coy_pix << " , " << max_coy_pix <<"]"<<endl;

	////////////
	////ETAPE2//
	////////////

	//ETAPE2: prendre tout les pixels dans ce carré, ils sont tous potentiellement dans le triangle

	for(int y=min_coy_pix; y<= max_coy_pix;y++){
		for(int x=min_cox_pix; x<= max_cox_pix;x++){
			int pix_index = (y*width)-(width-x); //indice du pixel dans le vecteur pixels
			double center_x = ((x*lg_pix)+min_x)-(lg_pix/2); //centre du pixel selon x
			double center_y = (max_y-(y*h_pix))+(h_pix/2); //centre du pixel selon y
			//cout << "Try : (" <<center_x<<","<<center_y<<")"<<endl;

			////////////
			////ETAPE3//
			////////////

			//ETAPE3: On color uniquement les pixels non colorés qui sont dans le triangle

			if (pixels[pix_index-1] == default_color && T.contain(center_x,center_y)){
				//Attribution de l'illumination (ombre) du triangle au pixel
				pixels_illumination[pix_index-1] = T.illumination;
				//Attribution d'une profondeur au pixel
				double depth_estime = T.compute_depth(center_x,center_y);
				//convertion de la profondeur en indice de couleur
				pixels[pix_index-1] = convert_to_color(depth_estime);
				//cout << "Colored pixel : "<<pix_index<<" (" <<center_x<<","<<center_y<<") = " << pixels[pix_index-1] <<endl;
			}
		}
	}
}

int convert_to_color(double value){
	/**
	* \brief Cette fonction converti une profondeur en indice de couleur.
	* \param value valeur à convertir en couleur
	* Cette fonction à aussi besoin des variables globales suivantes :
	* - nombre de couleurs voulues pour échantilloner la colormap (np_color)
	* - limites de profondeurs (min_depth, max_depth)
	*/

	double elongation = max_depth-min_depth;
	int color = max(0.0,(value-min_depth)*nb_colors/elongation);

	return color;
}

void compute_coords_y(int pixel_index,int &result,int width){
	/**
	* \brief Cette fonction calcul les coordonées d'un pixel sur y (en pixel).
	* \param pixel_index Indice du pixel concerné.
	* \param result Variable dans laquelle stocker le résultat.
	* \param width Nombre de pixels sur l'image en largeur.
	*/

	if (pixel_index%width == 0){ //gestion des valeur limites qui peuvent poer problème pour les indices
		result = min((pixel_index/width),width);
	}
	else{
		result = min((pixel_index/width)+1,width);
	}
}


int pixel_of_point(point &point){
	/**
	* \brief Cette fonction renvoie l'indice du pixel qui contient le point donné en paramètre.
	* \param point	Point pour lequel on veut le pixel correspondant.
	* Cette fonction à aussi besoin des variables globales suivantes :
	* - valeurs limites des positions des points (min_x,min_y,max_x,max_y)
	* - nombre de pixels voulus (width,height)
	* - largeur et hauteur des pixels en m (lg_pix,h_pix)
	*/

	int index;

	//le pixel d'indice 0 est celui situé en haut à gauche de l'image et celui d'indice maximale est en bas à droite de l'image
	
	//calcul du nombre de pixels à l'origine en abscisse du point :
	double elongation_x = abs(max_x-min_x); //elongation des mesures sur x
	double percent_x = abs(point.x-min_x)/elongation_x; //placement de x sur cette élongation en pourcentage
	int nb_pix_x = (percent_x*width)+1; //on applique ce pourcentage sur le nombre de pixels
	nb_pix_x = min(double(nb_pix_x),width); //sécurité pour valeur limites qui pose probblème

	//calcul du nombre de pixels à l'origine en ordonnée du point :
	double elongation_y = abs(max_y-min_y);
	double percent_y = 1-(abs(point.y-min_y)/elongation_y); //inversion pourcentage car min_y est à l'opposé du pixel d'indice 0 
	int nb_pix_y = (percent_y*height)+1; 
	nb_pix_y = min(double(nb_pix_y),height);

	//calcul de l'indice du pixel sur lequel il est :
	index = (nb_pix_y*width)-(width-nb_pix_x);

	return index;
}
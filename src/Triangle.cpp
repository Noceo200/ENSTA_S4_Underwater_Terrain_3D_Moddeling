#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <math.h>
#include <iostream> // bibliothèque d’entrées/sorties
#include "Triangle.h"

using namespace std;

/**
* \file Triangle.cpp
* \brief Fichier d'implémentation de la classe Triangle
* \date 04/01/2022
* \author NOEL Océan
*/

float sign (double x0, double y0, point *p_2, point *p_3);
double compute_illumination(std::vector<double> &light_dir, std::vector<double> &face_dir);

Triangle::Triangle(point* p_1,point* p_2,point* p_3)
{
	/**
	* \brief Constructeur de l'objet Triangle.
	* \param p1 1er point du triangle.
	* \param p2 2e point du triangle.
	* \param p3 3e point du triangle.
	*/
	p1 = p_1;
	p2 = p_2;
	p3 = p_3;

	//Implémentation de l'équation du plan du triangle
	//selection de deux cotés pour servir de vecteur directeurs du plan
	vector<double> v1 = {p3->x-p2->x,p3->y-p2->y,p3->depth-p2->depth}; //p2 --> p3
	vector<double> v2 = {p3->x-p1->x,p3->y-p1->y,p3->depth-p1->depth}; //p1 --> p3

	//calcul du vecteur normal
	vn = {v1[1]*v2[2]-v2[1]*v1[2],-v1[0]*v2[2]+v2[0]*v1[2],v1[0]*v2[1]-v2[0]*v1[1]};
}

void Triangle::compute_illumination(vector<double> &light_dir){
	/**
	* \brief Cette fonction calcul l'illumination du triangle par rapport à un vecteur de lumière.
	* - illumination de 1 : face illuminée au maximum 
	* - illumination de 0 : face non illuminée
	* \param light_dir direction de la lumière.
	*/

	//Normalisation des vecteurs
	double v1_norm = sqrt(pow(light_dir[0],2)+pow(light_dir[1],2)+pow(light_dir[2],2));
	double v2_norm = sqrt(pow(vn[0],2)+pow(vn[1],2)+pow(vn[2],2));
	vector<double> v1 = {light_dir[0]/v1_norm,light_dir[1]/v1_norm,light_dir[2]/v1_norm};
	vector<double> v2 = {-vn[0]/v2_norm,-vn[1]/v2_norm,-vn[2]/v2_norm};
	
	//Calcul de l'illumination
	illumination = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];//produit scalaire 3D

	//gestion et ajustement de l'illumination
	if (illumination > 0){ //on assombri très peu les faces qui recoivent la lumière de face inclinée
		double illu_min = 0.8; //illumination minimum voulue
		double illu_max = 1; //illumination maximal voulue
		double range = illu_max-illu_min;
		illumination = max(illu_min,illu_min+(pow(illumination,1.0/3)*range)); //nouvelle illumination
	}
	else{ //on assombri toutes les autres faces beaucoup plus, mais pas jusqu'à 100%
		double illu_min = 0.5; //illumination minimum voulue
		double illu_max = 0.8; //illumination maximale voulue
		double range = illu_max-illu_min;
		illumination = max(illu_min,illu_min+((1-pow(abs(illumination),1.0/2))*range)); //nouvelle illumination
	}
}

double Triangle::compute_depth(double xp, double yp){
	/**
	* \brief Calcul la profondeur d'un point n'importe où sur le triangle.
	* \param xp coordonnée x du point à calculer.
	* \param yp coordonnée y du point à calculer.
	* \return Valeur de la profondeur calculée.
	*/

	//calcul des coeffs et de la constante de l'équation du plan
	double a = vn[0];
	double b = vn[1];
	double c = vn[2];
	double d = -(a*p1->x+b*p1->y+c*p1->depth);
	//Plan : ax + by + cz + d = 0

	//calcul de la profondeur estimée du point
	double depth_estime = -(a*xp+b*yp+d)/c;

	return depth_estime;
}

bool Triangle::contain(double x, double y){
	/**
	* \brief Calcul si le point de coordonées x,y se trouve dans le triangle.
	* \param x coordonnée x du point à analyser.
	* \param y coordonnée y du point à analyser.
	* \return booléen.
	*/

    //calcul du coté duquel est le point par rapport aux trois cotés
    float c1 = sign(x,y, p1, p2); //négatif si le point est à gauche du coté (v1,v2)
    float c2 = sign(x,y, p2, p3);
    float c3 = sign(x,y, p3, p1);

 	//le point est dans le triangle seulement si il est du bon coté des segments
    bool has_neg = (c1 < 0) || (c2 < 0) || (c3 < 0);
    bool has_pos = (c1 > 0) || (c2 > 0) || (c3 > 0);

    return !(has_neg && has_pos);
}


float sign (double x0, double y0, point *p_2, point *p_3)
{
	/**
	* \brief Calcul si le point est à gauche ou à droite du segment (p_2,p_3).
	* \param x0 coordonnée x du point à analyser.
	* \param y0 coordonnée y du point à analyser.
	* \param p_2 1er point du segment.
	* \param p_3 2e point du segment.
	* \return float du résultat, négatif si le point est à gauche du coté (v1,v2), positif sinon.
	*/
    return (x0 - p_3->x) * (p_2->y - p_3->y) - (p_2->x - p_3->x) * (y0 - p_3->y);
}

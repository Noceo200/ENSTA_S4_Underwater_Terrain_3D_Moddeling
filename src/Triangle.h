#include "struct_point.h"

#ifndef OBJ_TRIANGLE_H
#define OBJ_TRIANGLE_H

/**
* \file Triangle.h
* \brief Fichier de déclaration de la classe Triangle
* \date 04/01/2022
* \author NOEL Océan
*/

class Triangle
{
/**
* \class Triangle
* \brief Classe Triangle qui permet d'attribuer une profondeur aux pixels qui lui appartiennent.
*/
public:
	Triangle(point* p1,point* p2,point* p3);
	bool contain(double x, double y);
	double compute_depth(double x, double y);
	void compute_illumination(std::vector<double> &light_dir);
	point* p1;
	point* p2;
	point* p3;
	double illumination = 1; //illumination du triangle, initialement, il est totalement illuminé
	std::vector<double> vn = {0,0,0}; //vecteur normal du plan du triangle 
};

#endif

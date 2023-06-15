#include <cstdlib> // bibliothèque générique standard
#include <iostream> // bibliothèque d’entrées/sorties
#include <string>
#include <unistd.h>
#include <vector> //vecteur
#include <map> //dictionnaires
#include "struct_point.h"

#ifndef INIT_POINT_PIXEL_H
#define INIT_POINT_PIXEL_H

/**
* \file init_points_pixels.h
* \brief Fichier de déclaration des fonctions utilisée pour initialiser les points et pixels.
* \date 04/01/2022
* \author NOEL Océan
*/

void create_pixels(std::vector<int> &pixels, std::vector<double> &pixels_illumination, std::map<std::string,double> &context);
void get_point(point& p,std::string& str);
int get_points(std::string file_name,std::vector<point> *v);
void project_points(std::vector<point> *v, std::vector<double> &points_line, std::map<std::string,double> &context);

#endif
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <map>
#include "struct_point.h"
#include "Triangle.h"
#include "struct_point.h"

/**
* \file triangulation.h
* \brief Fichier de déclaration des fonctions utilisées pour la triangulation
* \date 04/01/2022
* \author NOEL Océan
*/

#ifndef TRIANGULATION_H
#define TRIANGULATION_H

void triangulate_n_color(std::vector<point> &points, std::vector<double> &points_line,std::vector<int> &pixels, std::vector<double> &pixels_illumination,std::map<std::string,double> &context);
void find_pixels(Triangle &T,std::vector<int> &pixels, std::vector<double> &pixels_illumination);
int pixel_of_point(point &point);
void compute_coords_y(int pixel_index,int &result,int width);
int convert_to_color(double value);

#endif
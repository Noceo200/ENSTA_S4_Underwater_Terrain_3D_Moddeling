#include <string>
#include <unistd.h>
#include <ctime> //temps, mesures d'executions
#include <vector> //vecteur
#include <map> //dictionnaires

#ifndef GENERATE_IMAGE_H
#define GENERATE_IMAGE_H

/**
* \file generate_image.h
* \brief Fichier de déclaration des fonctions utilisées pour générer une image bianire en couleur.
* \date 04/01/2022
* \author NOEL Océan
*/

int generate_image(std::vector<int> &pixels, std::vector<double> &pixels_illumination, std::map<std::string,double> &context);
void DarkenColor(char &color, double darken_percent);

#endif
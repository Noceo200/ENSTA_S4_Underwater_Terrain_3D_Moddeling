#include <string>
#include <unistd.h>
#include <ctime> //temps, mesures d'executions
#include <vector> //vecteur
#include <map> //dictionnaires
#include <fstream> //manipulation fichiers
#include <vtkSmartPointer.h> //gestion colormap
#include <vtkColorTransferFunction.h> //gestion colormap
#include "generate_image.h"

/**
* \file generate_image.cpp
* \brief Fichier d'implémentation des fonctions utilisées pour générer une image binaire en couleur.
* \date 04/01/2022
* \author NOEL Océan
*/

using namespace std;

int generate_image(vector<int> &pixels, vector<double> &pixels_illumination, map<string,double> &context){
	/**
	* \brief Cette fonction génère une image bianire en couleur à partir d'une liste de pixels et d'une liste de couleur. 
	* \param pixels	Vecteur qui contient la couleur des pixels.
	* \param pixels_illumination Vecteur dans lequel sont stocké les illuminations des pixels (0 = non illuminé, 1 = illuminé).
	* \param context Dictionnaire qui contient les données importantes (evite les variables globales).
	* Ici cette fonction à besoin de :
	* - nombre de pixels voulus (width,height)
	* - nombre de couleurs dans le color_map (nb_colors)
	*/

	////////////////////
	////INITIALISATION//
	////////////////////

	//récupération des variables nécessaires
	int width = context["width"];
	int height = context["height"];
	int nb_colors = context["nb_colors"];

	//Initialisation de la colomap :
	// Création d'une colormap Haxby en définissant une fonction de transfert de couleur
  	vtkSmartPointer<vtkColorTransferFunction> haxbyColorMap = vtkSmartPointer<vtkColorTransferFunction>::New();

  	//définition des points de couleurs de la colormap
	haxbyColorMap->AddRGBPoint(1.0, 1.0, 0.5, 0.0); //map la valeur 1.0 à la couleur (1,0.5,0) = (255,127,0)RGB
	haxbyColorMap->AddRGBPoint(0.65, 1.0, 1.0, 0.6);
	haxbyColorMap->AddRGBPoint(0.5, 0.6, 1.0, 0.6);
	haxbyColorMap->AddRGBPoint(0.35, 0.0, 0.9, 1.0);
	haxbyColorMap->AddRGBPoint(0.0, 0.0, 0.1, 0.4);

	//variables analytiques
	time_t t0,tf; 
	time(&t0);

	//nom de l'image
	string image_name = "raster.ppm";

	//initialisation de l'entête de l'image
	ofstream myImage;		//output stream object
	myImage.open(image_name); //tentative d'ouverture/création de l'image
	if (myImage.fail())
	{
		cout << "Impossible de créer " << image_name << endl;
		return 0; //arret du programme si echec
	}
	{ //Image header - Need this to start the image properties
		myImage << "P6" << endl;						//Declare that you want to use binary colour values
		myImage << width << " " << height << endl;		//Declare w & h
		myImage << "255" << endl;						//Declare max colour ID
	}

	////////////////
	////OPERATIONS//
	////////////////

	//variables pour gestion de la progression
	time(&t0);
	double compteur = 0;
	int progress = 0;

	//generation de l'image
	cout<<endl<<"Image generation...";
	for(int i = 0; i < pixels.size(); ++i) //on parcours la liste de pixels et leur illumination associée
	{
		//recupération des valeurs
		int pixel_color = pixels[i]; //indice de couleur du pixel
		double pixel_illu = pixels_illumination[i]; //illumination du pixel

		//conversion des indices de couleur en couleurs
		double value = double(pixel_color)/double(nb_colors); //on se ramène à une valeur entre 0 et 1
		unsigned char* color = haxbyColorMap->MapValue(value); //conversion en couleur via la colormap définie avant

		//récupération des couleurs sous forme binaire
		char r = color[0]; //rouge
		char g = color[1]; //vert
		char b = color[2]; //bleu
		
		//ajout des ombres
		if(pixel_illu != 1){ //si il faut assombrir le pixel...
			DarkenColor(r,pixel_illu);
			DarkenColor(g,pixel_illu);
			DarkenColor(b,pixel_illu);
		}

		if (pixel_color == 0){ //on garde les pixels à 0 en noir
			r = static_cast<char>(0); //rouge
			g = static_cast<char>(0); //vert
			b = static_cast<char>(0); //bleu
		}

		//enregistrement
		myImage << r << g << b;

		//progression
		compteur +=1;
		progress = 100*compteur/(width*height);
		string progress_str = to_string(progress);
		cout << progress_str << " %";
		cout << "\b\b";
		for(int i=0; i<progress_str.size();i++){
			cout << "\b";
		}
	}
	myImage.close();

	time(&tf);
	cout<<" ("<<tf-t0<<" s)"<<endl; //affichage du temps d'execution
	
	return 1;
}

void DarkenColor(char &color, double darken_percent){
	/**
	* \brief Cette fonction assombri une couleur de 1-darken_percent. 
	* \param color	Vecteur qui contient la couleur à plus ou moins illuminer.
	* \param darken_percent Valeur entre 0 et 1 de l'illumination voulue pour la couleur.
	*/

	//conversion de la couleur en code RGB entre 0 et 255
	int COLOR = 0;
	if(int(color) < 0){ //int(color) donne des valeurs brut positives ou négatives qu'il faut traiter
		COLOR = 255 + int(color);
	}
	else if(int(color) > 0){
		COLOR = int(color);
	}

	//assombrissement
	COLOR = COLOR*darken_percent;

	//enregistrement
	color = static_cast<char>(COLOR);
}
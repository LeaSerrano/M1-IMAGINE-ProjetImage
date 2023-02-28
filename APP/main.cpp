#include "Library/ImageBase.h"
#include "Library/PerlinNoise.hpp"
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace siv;

int main(int argc, char **argv)
{
	cout << "---------------------------------\n";
	cout << "* frequency [0.1 .. 8.0 .. 64.0] \n";
	cout << "* octaves   [1 .. 8 .. 16]       \n";
	cout << "* seed      [0 .. 2^32-1]        \n";
	cout << "---------------------------------\n";

	double frequency = 16.0;
	cout << "double frequency = ";
	//cin >> frequency;
	frequency = clamp(frequency, 0.1, 64.0);

	int32_t octaves = 1;
	cout << "int32 octaves    = ";
	//cin >> octaves;
	octaves = clamp(octaves, 1, 16);

	uint32_t seed = 45000;
	cout << "uint32 seed      = ";
	//cin >> seed;

	const PerlinNoise perlin{seed};

	int width = 512; int height = 512;
	double fx = frequency / width; double fy = frequency / height;

	ImageBase* image = new ImageBase(width,height,false);

	for(int y = 0; y < height ; y++)
	{
		for(int x = 0; x < width; x++)
		{
			double v = perlin.octave2D_01(x*fx,y*fy,octaves);
			image->set(x,y,0,v*255);
		}
	}

	image->save("perlin.pgm");

	return 0;
}

/*
///////////////////////////////////////// Exemple d'un seuillage d'image
	char cNomImgLue[250], cNomImgEcrite[250];
	int S;
  
	if (argc != 4) 
	{
		printf("Usage: ImageIn.pgm ImageOut.pgm Seuil \n"); 
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue) ;
	sscanf (argv[2],"%s",cNomImgEcrite);
	sscanf (argv[3],"%d",&S);
	
	
	//ImageBase imIn, imOut;
	ImageBase imIn;
	imIn.load(cNomImgLue);

	//ImageBase imG(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

	for(int x = 0; x < imIn.getHeight(); ++x)
		for(int y = 0; y < imIn.getWidth(); ++y)
		{
			if (imIn[x][y] < S) 
				imOut[x][y] = 0;
			else imOut[x][y] = 255;
		}
		
	imOut.save(cNomImgEcrite);
		

	
	
	///////////////////////////////////////// Exemple de création d'une image couleur
	ImageBase imC(50, 100, true);

	for(int y = 0; y < imC.getHeight(); ++y)
		for(int x = 0; x < imC.getWidth(); ++x)
		{
			imC[y*3][x*3+0] = 200; // R
			imC[y*3][x*3+1] = 0; // G
			imC[y*3][x*3+2] = 0; // B
		}
		
	imC.save("imC.ppm");
		



	///////////////////////////////////////// Exemple de création d'une image en niveau de gris
	ImageBase imG(50, 100, false);

	for(int y = 0; y < imG.getHeight(); ++y)
		for(int x = 0; x < imG.getWidth(); ++x)
			imG[y][x] = 50;

	imG.save("imG.pgm");




	ImageBase imC2, imG2;
	
	///////////////////////////////////////// Exemple lecture image couleur
	imC2.load("imC.ppm");
	///////////////////////////////////////// Exemple lecture image en niveau de gris
	imG2.load("imG.pgm");
	
	

	///////////////////////////////////////// Exemple de récupération d'un plan de l'image
	ImageBase *R = imC2.getPlan(ImageBase::PLAN_R);
	R->save("R.pgm");
	delete R;
	*/

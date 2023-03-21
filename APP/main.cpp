#include "Library/ImageBase.h"
#include "Library/PerlinNoise.hpp"

#include "App/MapManager.h"
#include "App/ProjectManager.h"
#include "App/DataManager.hpp"
#include "App/HeightMap.hpp"

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#include <string>

using namespace std;
using namespace siv;

MapManager* mapManager;
ProjectManager* projectManager;
DataManager* dataManager;

void makeMap(string id)
{
	do
	{
		mapManager->requestMap(id); mapManager->saveMap(id);

		cout << ">> Satisfied with this map ? (y/n) : ";
		string line; getline(std::cin, line);
		if(line != "n")	{	break;	}
		else
		{
			mapManager->deleteMap(id);
		}
	}while(true);
}


void tests()
{
	/*
	int* histo = new int[256];
	for(int i = 0; i < 256; i++)
	{
		histo[i] = (int)(landCurve(i/255.0,0.5,0.1,0.45,0.65,0.9)*255);
	}

	ofstream flux("data.dat");
	for(int i = 0; i < 256;i++)	{	flux << i << " " << histo[i] << std::endl;}*/
}

int main(int argc, char **argv)
{
	srand (time(NULL));

	dataManager = new DataManager();

	mapManager = new MapManager();

	if(argc >= 2)
	{
		projectManager = new ProjectManager(argv[1]);
	}
	else
	{
		projectManager = new ProjectManager();
	}

	tests();

	cout << endl << "--   --  Welcome to Map Maker  --   --" << endl << endl;

	cout << "--   Lets start with some main parameters : " << endl;

	dataManager->setupValue("map_size");
	dataManager->setupValue("map_scale");

	cout << endl << "--  Lets start by generating the base height map :" << endl;
	makeMap("HEIGHT_LARGE");

	makeMap("HEIGHT_SMALL");

	makeMap("HEIGHT_BASE");

	cout << endl << "--  Choose sea-level :" << endl;

	makeMap("SEA_BINARY");

	cout << endl << "--  Lets refine the height map with sea-parameters :" << endl;
	makeMap("HEIGHT_SEA");

	cout << endl << "--  Lets refine the height map with land-parameters :" << endl;
	makeMap("HEIGHT_LAND");

	cout << endl << "--  Generate the gradient map :" << endl;
	makeMap("HEIGHT_GRADIENT");

	cout << endl << "--  Generate the river map :" << endl;
	makeMap("RIVER");

	cout << endl << "--  Generate the reliefs map :" << endl;
	makeMap("RELIEF");

	mapManager->saveAllMaps();

	projectManager->saveData();

	return 0;
}

/*
 * cout << "---------------------------------\n";
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

	image->save("perlin.pgm");*/

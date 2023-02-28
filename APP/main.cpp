#include "Library/ImageBase.h"
#include "Library/PerlinNoise.hpp"

#include "App/MapManager.hpp"
#include "App/ProjectManager.hpp"

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

using namespace std;
using namespace siv;

MapManager* mapManager;
ProjectManager* projectManager;

int main(int argc, char **argv)
{
	projectManager = new ProjectManager();

	mapManager = new MapManager();

	mapManager->generateHeightMap(512,512);

	mapManager->saveMap("HEIGHT");

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

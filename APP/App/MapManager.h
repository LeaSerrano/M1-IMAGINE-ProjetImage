#ifndef MAPMANAGER
#define MAPMANAGER

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <map>
#include <string>

using namespace std;
using namespace siv;

class MapManager
{
private:

    map<string,ImageBase*> maps;

public:

    uint32_t seed() {return (rand() / RAND_MAX) * pow(2,32); }

    void generateHeightMap(int width, int height)
    {
        ImageBase* heightMap = new ImageBase(width,height,false);

        double frequency = 16.0;
        clamp(frequency, 0.1, 64.0);

        int32_t octaves = 1;
        octaves = clamp(octaves, 1, 16);

        const PerlinNoise perlin{seed()};

        double fx = frequency / width; double fy = frequency / height;

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                double v = perlin.octave2D_01(x*fx,y*fy,octaves);
                heightMap->set(x,y,0,v*255);
            }
        }

        maps["HEIGHT"] = heightMap;
    }

    void saveMap(string id);

    void loadMap(string name);

    MapManager()
    {
        instance = this;
        srand (time(NULL));
    }

    inline static MapManager* instance;
};

#endif
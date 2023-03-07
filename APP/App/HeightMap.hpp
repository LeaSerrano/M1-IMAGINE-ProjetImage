#pragma once

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"
#include "MapManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <string>

using namespace std;
using namespace siv;

class HeighMap
{

public:

    static ImageBase* generatePerlin(int width, int height, double scale, int octaves)
    {
        uint32_t s = MapManager::seed();
        const PerlinNoise perlin{s};
        double fr = 1.0 / scale;
        
        ImageBase* map = new ImageBase(width,height,false);

        double fx = fr / width; double fy = fr / height;

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                double v = perlin.octave2D_01(x*fx,y*fy,octaves);
                map->set(x,y,0,v*255);
            }
        }

        return map;
    }

    static ImageBase* generateHeightMap(int width, int height)
    {
        ImageBase* heightMap = new ImageBase(width,height,false);

        ImageBase* large = generatePerlin(width,height,0.25,1);

        for(int i = 0; i < heightMap->getSize(); i++)
        {
            heightMap->set(i,0,large->get(i,0));
        }

        return heightMap;
    }

};
#pragma once

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"
#include "MapManager.h"
#include "DataManager.hpp"

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

    static ImageBase* generateHeightMap()
    {
        int width = DataManager::instance->getValue("map_size"); int height = width;

        ImageBase* heightMap = new ImageBase(width,height,false);

        ImageBase* large = generatePerlin(width,height,0.25,1);

        for(int i = 0; i < heightMap->getSize(); i++)
        {
            heightMap->set(i,0,large->get(i,0));
        }

        return heightMap;
    }

    static ImageBase* seuilHeightMap(ImageBase* heightMap) {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        float sea_level = DataManager::instance->requestValue("sea_level");
        float sea_slope = DataManager::instance->requestValue("sea_slope");

        ImageBase* outImg = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                if (value < sea_level) 
                {
                    value = pow(value, sea_slope);
                }

                outImg->set(x,y,0,(int)(value*255));
            }
        }

        return outImg;
    }

};
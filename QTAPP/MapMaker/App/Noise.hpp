#pragma once

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>

using namespace std;
using namespace siv;

class Noise
{
public:


    static ImageBase* generatePerlin(int width, int height, double scale, int octaves)
    {
        uint32_t s = MapManager::seed();
        const PerlinNoise perlin{s};
        double fr = scale/33.0;
        
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


};
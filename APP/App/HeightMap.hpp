#pragma once

#include "../Library/ImageBase.h"
#include "Noise.hpp"
#include "MapManager.h"
#include "DataManager.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <vector>
#include <string>
#include <math.h>

using namespace std;
using namespace siv;

class HeightMap
{

public:

    static ImageBase* remap(ImageBase* img, int min, int max)
    {
        ImageBase* map = new ImageBase(img->getWidth(),img->getHeight(),false);

        int imax = 0; int imin = 0;
        for(int i = 0; i < img->getSize();i++)
        {
            if(img->get(i,0) > imax) {imax = img->get(i,0);}
            if(img->get(i,0) < imin) {imin = img->get(i,0);}
        }

        for(int i = 0; i < img->getSize();i++)
        {
            float v = img->get(i,0);
            v -= imin;
            v /= (float)(imax-imin);

            v *= (max-min);
            v += min;
            map->set(i,0,v);
        }

        return map;
    }

    static int quantile(ImageBase* img, double ratio)
    {
        vector<int> values;
        for(int i = 0; i < img->getSize();i++)
        {
            values.push_back(img->get(i,0));
        }

        sort(values.begin(), values.end());

        return values.at((values.size()-1) * ratio);
    }

    static ImageBase* baseMap()
    {
        int width = DataManager::instance->requestValue("map_size"); int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* large = Noise::generatePerlin(width,height,scale,1);

        large = remap(large,0,255);

        return large;
    }

    static double seaCurve(double value,double sea_level,double sea_slope,double shore_width)
    {
        value /= sea_level; //Normalisation [0 - sealevel] > [0 - 1]
        if(value < 1.0-shore_width)
        {
            value /= (1.0-shore_width); //Normalisation [0 - shore] > [0 - 1]
            value = pow(value,sea_slope);
            value *= (1.0-shore_width);
        }
        else
        {
            value -= (1-shore_width);
            value /= shore_width;
            value = pow(value,1.0/sea_slope);
            value *= shore_width;
            value += (1-shore_width);
        }
        return value * sea_level;
    }

    static ImageBase* seaMap(ImageBase* heightMap) {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        float sea_level = quantile(heightMap,0.7) / 255.0;
        sea_level = DataManager::instance->requestValue("sea_level",sea_level);
        float sea_slope = DataManager::instance->requestValue("sea_slope");
        float shore_width = DataManager::instance->requestValue("shore_width");

        ImageBase* outImg = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                if (value < sea_level) 
                {
                    value = seaCurve(value,sea_level,sea_slope,shore_width);
                }

                outImg->set(x,y,0,(int)(value*255));
            }
        }

        return outImg;
    }

};

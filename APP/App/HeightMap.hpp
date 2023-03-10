#pragma once

#include "../Library/ImageBase.h"
#include "Noise.hpp"
#include "Utilities.hpp"
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

    static ImageBase* baseMap()
    {
        int width = DataManager::instance->requestValue("map_size"); int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* large = Noise::generatePerlin(width,height,scale,1);

        large = Utilities::remap(large,0,255);

        return large;
    }

    static double seaCurve(double value,double sea_level,double sea_slope,double shore_level)
    {
        value /= sea_level; //Normalisation [0 - sealevel] > [0 - 1]
        if(value < 1.0-shore_level)
        {
            value /= (1.0-shore_level); //Normalisation [0 - shore] > [0 - 1]
            value = pow(value,sea_slope);
            value *= (1.0-shore_level);
        }
        else
        {
            value -= (1-shore_level);
            value /= shore_level;
            value = pow(value,1.0/sea_slope);
            value *= shore_level;
            value += (1-shore_level);
        }
        return value * sea_level;
    }

    static ImageBase* applySea(ImageBase* heightMap)
    {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        float sea_level = DataManager::instance->requestValue("sea_level");
        float sea_slope = DataManager::instance->requestValue("sea_slope",true);
        float shore_level = DataManager::instance->requestValue("shore_level",true);

        ImageBase* outImg = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                if (value < sea_level) 
                {
                    value = seaCurve(value,sea_level,sea_slope,shore_level);
                }

                outImg->set(x,y,0,(int)(value*255));
            }
        }

        return outImg;
    }

    static ImageBase* seaBinaryMap(ImageBase* heightMap)
    {
        ImageBase* outImg = new ImageBase(heightMap->getWidth(),heightMap->getHeight(),false);

        float sea_level = Utilities::quantile(heightMap,0.5) / 255.0;
        sea_level = DataManager::instance->requestValue("sea_level",sea_level,true);
        for(int i = 0; i < heightMap->getSize();i++)
        {
            if(heightMap->get(i,0) / 255.0 < sea_level)
            {
                outImg->set(i,0,0);
            }
            else
            {
                outImg->set(i,0,255);
            }
        }

        return outImg;
    }

    static ImageBase* applyLand(ImageBase* heightMap)
    {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        float sea_level = DataManager::instance->requestValue("sea_level");
        // some other parameters
        //  i.e : beach_level , plain_level , plateau_level , pikes_level

        ImageBase* outImg = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                if (value >= sea_level)
                {
                    //value = landCurve .... (method to create)
                }

                outImg->set(x,y,0,(int)(value*255));
            }
        }

        return outImg;
    }

    static ImageBase* gradientMap(ImageBase* heightMap)
    {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        //PPM image : gradient will be stored as a vector + norm using RGB (dx , dy , norm)
        ImageBase* outImg = new ImageBase(width,height,true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                //calculate the gradient direction

                outImg->set(x,y,0,(int)(value*255));
            }
        }

        return outImg;
    }

};

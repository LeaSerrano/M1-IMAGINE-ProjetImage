#pragma once

#include "../Library/ImageBase.h"
#include "Noise.hpp"
#include "Utilities.hpp"
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

#define suggestedSeaRatio 0.5

#define suggestedBeachLevel 0.1
#define suggestedPlainLevel 0.45
#define suggestedHillsLevel 0.65
#define suggestedPlateauLevel 0.9

#define suggestedBeachCurve 2
#define suggestedPlainCurve 1.3
#define suggestedHillsFrequency 5
#define suggestedHillsAmplitude 0.03
#define suggestedPlateauCurve 0.5
#define suggestedPeaksCurve 4


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

        float sea_level = Utilities::quantile(heightMap,suggestedSeaRatio) / 255.0;
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


    static double landCurve(   double value, double sea_level,
                        double beach_level, double beach_curve, // 2
                        double plain_level, double plain_curve, // 1.3
                        double hills_level, double hills_frequency, double hills_amplitude, // 0.1 5
                        double plateau_level, double plateau_curve, double peaks_curve) // 0.5 4
    {
        if(value < sea_level)
        {
            return value;
        }

        value -= sea_level;
        value /= (1.0-sea_level);

        if(value < beach_level)
        {
            value /= beach_level;
            value = pow(value,beach_curve);
            value *= beach_level;
        }
        else if(value < plain_level)
        {
            value -= beach_level;
            value /= (plain_level - beach_level);
            value = pow(value,1.3);
            value *= (plain_level - beach_level);
            value += beach_level;
        }
        else if(value < hills_level)
        {
            value -= plain_level;
            value /= (hills_level - plain_level);
            value += 0.1 * sin(value * M_PI * 5);
            value *= (hills_level - plain_level);
            value += plain_level;
        }
        else if(value < plateau_level)
        {
            value -= hills_level;
            value /= (plateau_level - hills_level);
            value = pow(value,plateau_curve);
            value *= (plateau_level - hills_level);
            value += hills_level;
        }
        else
        {
            value -= plateau_level;
            value /= (1 - plateau_level);
            value = pow(value,peaks_curve);
            value *= (1 - plateau_level);
            value += plateau_level;
        }

        value *= (1.0-sea_level);
        value += sea_level;
        return value;
    }

    static ImageBase* applyLand(ImageBase* heightMap)
    {
        int width = heightMap->getWidth();
        int height = heightMap->getHeight();

        float sea_level = DataManager::instance->requestValue("sea_level");

        float beach_level = suggestedBeachLevel;
        beach_level = DataManager::instance->requestValue("beach_level",beach_level,true);
        float beach_curve = DataManager::instance->requestValue("beach_curve",suggestedBeachCurve,true);

        float plain_level = suggestedPlainLevel;
        plain_level = DataManager::instance->requestValue("plain_level",plain_level,true);
        float plain_curve = DataManager::instance->requestValue("plain_curve",suggestedPlainCurve,true);

        float hills_level = suggestedHillsLevel;
        hills_level = DataManager::instance->requestValue("hills_level",hills_level,true);
        float hills_frequency = DataManager::instance->requestValue("hills_frequency",suggestedHillsFrequency,true);
        float hills_amplitude = DataManager::instance->requestValue("hills_amplitude",suggestedHillsAmplitude,true);

        float plateau_level = suggestedPlateauLevel;
        plateau_level = DataManager::instance->requestValue("plateau_level",plateau_level,true);
        float plateau_curve = DataManager::instance->requestValue("plateau_curve",suggestedPlateauCurve,true);
        float peaks_curve = DataManager::instance->requestValue("peaks_curve",suggestedPeaksCurve,true);

        ImageBase* outImg = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float value = (float)(heightMap->get(x, y, 0)) / 255.0;

                value = landCurve(  value,sea_level,
                                        beach_level,beach_curve,
                                        plain_level,plain_curve,
                                        hills_level,hills_frequency,hills_amplitude,
                                        plateau_level,plain_curve,peaks_curve);

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

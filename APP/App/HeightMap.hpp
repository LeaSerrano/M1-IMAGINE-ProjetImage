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

     static ImageBase* generateLargeImage()
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* large = Noise::generatePerlin(width,height,scale,1);

        return large;
    }

    static ImageBase* generateSmallImage()
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* small = Noise::generatePerlin(width,height,scale*2,1);

        return small;
    }

    static ImageBase* baseMap(ImageBase* imageLarge, ImageBase* imageSmall)
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        float sea_level = DataManager::instance->requestValue("sea_level");

        float distorsionValue = 0.5;//DataManager::instance->requestValue("distorsion");

        ImageBase* image = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {   
                float valueLarge = (float)(imageLarge->get(x, y, 0))/255;
                float valueSmall = (float)(imageSmall->get(x, y, 0))/255;

                float newValue = (4*valueLarge + valueSmall)/5;

                image->set(x,y,0,(int)(newValue*255));
            }
        }

        image = Utilities::remap(image,0,255);

        return image;
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

        float dx, dy, norm;
        float valueUp1, valueUp2, valueUp3, valueRight1, valueRight2, valueRight3, valueDiag1, valueDiag2, valueDiag3, value;


        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {
                if (x < width-3 && y < height-3) {

                value = (float)(heightMap->get(x, y, 0));

                valueRight1 = (float)(heightMap->get(x+1, y, 0));
                valueRight2 = (float)(heightMap->get(x+2, y, 0));
                valueRight3 = (float)(heightMap->get(x+3, y, 0));

                valueUp1 = (float)(heightMap->get(x, y+1, 0));
                valueUp2 = (float)(heightMap->get(x, y+2, 0));
                valueUp3 = (float)(heightMap->get(x, y+3, 0));

                valueDiag1 = (float)(heightMap->get(x+1, y+1, 0));
                valueDiag2 = (float)(heightMap->get(x+2, y+2, 0));
                valueDiag3 = (float)(heightMap->get(x+3, y+3, 0));


                float dx1 = x - (x+1);
                float dy1 = y - (y+1);
                float dxDiag1 = x - (x+1);
                float dyDiag1 = y - (y+1);

                dx1 *= valueRight1 - value;
                dy1 *= valueUp1 - value;
                dxDiag1 *= valueDiag1 - value;
                dyDiag1 *= valueDiag1 - value;


                float dx2 = x - (x+2);
                float dy2 = y - (y+2);
                float dxDiag2 = x - (x+2);
                float dyDiag2 = y - (y+2);

                dx2 *= valueRight2 - value;
                dy2 *= valueUp2 - value;
                dxDiag2 *= valueDiag2 - value;
                dyDiag2 *= valueDiag2 - value;


                float dx3 = x - (x+3);
                float dy3 = y - (y+3);
                float dxDiag3 = x - (x+3);
                float dyDiag3 = y - (y+3);

                dx3 *= valueRight3 - value;
                dy3 *= valueUp3 - value;
                dxDiag3 *= valueDiag3 - value;
                dyDiag3 *= valueDiag3 - value;

                
                dx = (dx1 + dx2 + dx3 + dxDiag1 + dxDiag2 + dxDiag3)/6;
                dy = (dy1 + dy2 + dy3 + dyDiag1 + dyDiag2 + dyDiag3)/6;
                norm = sqrt(pow(dx, 2) + pow(dy, 2));

                dx = 128 + (dx * 128);
                dy = 128 + (dy * 128);
                norm *= 255;

                if (dx < 0) {
                    dx = 0;
                }
                else if (dx > 255) {
                    dx = 255;
                }
                if (dy < 0) {
                    dy = 0;
                }
                else if (dy > 255) {
                    dy = 255;
                }
                if (norm < 0) {
                    norm = 0;
                }
                else if (norm > 255) {
                    norm = 255;
                }

                outImg->set(x,y,0,(int)(dx));
                outImg->set(x,y,1,(int)(dy));
                outImg->set(x,y,2,(int)(norm));

                }
            }
        }

        //Image blurring
        for (int elt = 0; elt < 3; elt++) {

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
        
                    outImg->set(x,y,0,(int)((float)(outImg->get(x-2, y-2, 0)) + (float)(outImg->get(x-1, y-2, 0)) + (float)(outImg->get(x, y-2, 0)) + (float)(outImg->get(x+1, y-2, 0)) + (float)(outImg->get(x+2, y-2, 0)) + (float)(outImg->get(x-2, y-1, 0)) + (float)(outImg->get(x-1, y-1, 0)) + (float)(outImg->get(x, y-1, 0)) + (float)(outImg->get(x+1, y-1, 0)) + (float)(outImg->get(x+2, y-1, 0)) + (float)(outImg->get(x-2, y, 0)) + (float)(outImg->get(x-1, y, 0)) + (float)(outImg->get(x, y, 0)) + (float)(outImg->get(x+1, y, 0)) + (float)(outImg->get(x+2, y, 0)) + (float)(outImg->get(x-2, y+1, 0)) + (float)(outImg->get(x-1, y+1, 0)) + (float)(outImg->get(x, y+1, 0)) + (float)(outImg->get(x+1, y+1, 0)) + (float)(outImg->get(x-2, y+2, 0)) + (float)(outImg->get(x-1, y+2, 0)) + (float)(outImg->get(x, y+2, 0)) + (float)(outImg->get(x+1, y+2, 0)) + (float)(outImg->get(x+2, y+2, 0)))/25);
                    outImg->set(x,y,1,(int)((float)(outImg->get(x-2, y-2, 1)) + (float)(outImg->get(x-1, y-2, 1)) + (float)(outImg->get(x, y-2, 1)) + (float)(outImg->get(x+1, y-2, 1)) + (float)(outImg->get(x+2, y-2, 1)) + (float)(outImg->get(x-2, y-1, 1)) + (float)(outImg->get(x-1, y-1, 1)) + (float)(outImg->get(x, y-1, 1)) + (float)(outImg->get(x+1, y-1, 1)) + (float)(outImg->get(x+2, y-1, 1)) + (float)(outImg->get(x-2, y, 1)) + (float)(outImg->get(x-1, y, 1)) + (float)(outImg->get(x, y, 1)) + (float)(outImg->get(x+1, y, 1)) + (float)(outImg->get(x+2, y, 1)) + (float)(outImg->get(x-2, y+1, 1)) + (float)(outImg->get(x-1, y+1, 1)) + (float)(outImg->get(x, y+1, 1)) + (float)(outImg->get(x+1, y+1, 1)) + (float)(outImg->get(x-2, y+2, 1)) + (float)(outImg->get(x-1, y+2, 1)) + (float)(outImg->get(x, y+2, 1)) + (float)(outImg->get(x+1, y+2, 1)) + (float)(outImg->get(x+2, y+2, 1)))/25);
                    outImg->set(x,y,2,(int)((float)(outImg->get(x-2, y-2, 2)) + (float)(outImg->get(x-1, y-2, 2)) + (float)(outImg->get(x, y-2, 2)) + (float)(outImg->get(x+1, y-2, 2)) + (float)(outImg->get(x+2, y-2, 2)) + (float)(outImg->get(x-2, y-1, 2)) + (float)(outImg->get(x-1, y-1, 2)) + (float)(outImg->get(x, y-1, 2)) + (float)(outImg->get(x+1, y-1, 2)) + (float)(outImg->get(x+2, y-1, 2)) + (float)(outImg->get(x-2, y, 2)) + (float)(outImg->get(x-1, y, 2)) + (float)(outImg->get(x, y, 2)) + (float)(outImg->get(x+1, y, 2)) + (float)(outImg->get(x+2, y, 2)) + (float)(outImg->get(x-2, y+1, 2)) + (float)(outImg->get(x-1, y+1, 2)) + (float)(outImg->get(x, y+1, 2)) + (float)(outImg->get(x+1, y+1, 2)) + (float)(outImg->get(x-2, y+2, 2)) + (float)(outImg->get(x-1, y+2, 2)) + (float)(outImg->get(x, y+2, 2)) + (float)(outImg->get(x+1, y+2, 2)) + (float)(outImg->get(x+2, y+2, 2)))/25);
                } 
            } 
        }

        return outImg;
    }

    static ImageBase* getImageR(ImageBase* image)
    {
        int width = image->getWidth();
        int height = image->getHeight();

        ImageBase* outImg = new ImageBase(width,height,false);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                outImg->set(x,y,0, (image->get(x, y, 0)));
            }
        }

        return outImg;
    }

    static ImageBase* getImageG(ImageBase* image)
    {
        int width = image->getWidth();
        int height = image->getHeight();

        ImageBase* outImg = new ImageBase(width,height,false);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                outImg->set(x,y,0, (image->get(x, y, 1)));
            }
        }

        return outImg;
    }

    static ImageBase* getImageB(ImageBase* image)
    {
        int width = image->getWidth();
        int height = image->getHeight();

        ImageBase* outImg = new ImageBase(width,height,false);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                outImg->set(x,y,0, (image->get(x, y, 2)));
            }
        }

        return outImg;
    }

};

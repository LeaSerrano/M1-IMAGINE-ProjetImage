#pragma once

#include "../Library/ImageBase.h"
#include "Noise.hpp"
#include "Utilities.hpp"
#include "DataManager.hpp"
#include "ReliefMap.hpp"

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


class BiomeMap
{

public:

    inline static Color desert_plain_color = Color(255,255,50);;
    inline static Color desert_hills_color = Color(255,200,50);;
    inline static Color desert_plateau_color = Color(150,100,80);;

    inline static Color jungle_plain_color = Color(4, 110, 8);;
    inline static Color jungle_hills_color = Color(15, 94, 17);;
    inline static Color jungle_plateau_color = Color(76, 63, 19);;

    inline static Color snow_plain_color = Color(250,250,255);;
    inline static Color snow_hills_color = Color(188, 188, 209);;
    inline static Color snow_plateau_color = Color(130, 130, 173);;

    inline static Color ice_plain_color = Color(89, 189, 235);;
    inline static Color ice_hills_color = Color(39, 122, 163);;
    inline static Color ice_plateau_color = Color(17, 84, 120);;

    static bool isAReliefMapColor(int x, int y, ImageBase* image) {

        if (isTheSameColor(x, y, image, ReliefMap::plain_color) || isTheSameColor(x, y, image, ReliefMap::hills_color) || isTheSameColor(x, y, image, ReliefMap::plateau_color)) {
            return true;
        }

        return false;

    }

    static bool isABiomeMapColor(int x, int y, ImageBase* image) {

        if (isTheSameColor(x, y, image, desert_plain_color) || isTheSameColor(x, y, image, desert_hills_color) || isTheSameColor(x, y, image, desert_plateau_color) 
        || isTheSameColor(x, y, image, jungle_plain_color) || isTheSameColor(x, y, image, jungle_hills_color) || isTheSameColor(x, y, image, jungle_plateau_color)
        || isTheSameColor(x, y, image, snow_plain_color) || isTheSameColor(x, y, image, snow_hills_color) || isTheSameColor(x, y, image, snow_plateau_color)
        || isTheSameColor(x, y, image, ice_plain_color) || isTheSameColor(x, y, image, ice_hills_color) || isTheSameColor(x, y, image, ice_plateau_color)) {
            return true;
        }

        return false;

    }

    static bool isTheSameColor(int x, int y, ImageBase* image, Color color) {
        if (image->get(x, y, 0) == color.r && image->get(x, y, 1) == color.g && image->get(x, y, 2) == color.b) {
            return true;
        }
        return false;
    }

    static ImageBase* generateBiomeMap(ImageBase* reliefMap, ImageBase* climateMap,bool useDefaults) {

        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        float hot_temperature = Utilities::quantile(climateMap,0.7) / 255.0;
        if(!useDefaults)hot_temperature = DataManager::instance->requestValue("hot_temperature",hot_temperature,true);
        else DataManager::instance->setValue("hot_temperature",hot_temperature);

        float cold_temperature = Utilities::quantile(climateMap,0.3) / 255.0;
        if(!useDefaults)cold_temperature = DataManager::instance->requestValue("cold_temperature",cold_temperature,true);
        else DataManager::instance->setValue("cold_temperature",cold_temperature);

        float high_humidity = Utilities::quantile(climateMap,0.7) / 255.0;
        if(!useDefaults)high_humidity = DataManager::instance->requestValue("high_humidity",high_humidity,true);
        else DataManager::instance->setValue("high_humidity",high_humidity);

        float low_humidity = Utilities::quantile(climateMap,0.3) / 255.0;
        if(!useDefaults)low_humidity = DataManager::instance->requestValue("low_humidity",low_humidity,true);
        else DataManager::instance->setValue("low_humidity",low_humidity);

        ImageBase* biomeMap = new ImageBase(width, height, true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            { 
                biomeMap->set(x, y, 0, reliefMap->get(x, y, 0));
                biomeMap->set(x, y, 1, reliefMap->get(x, y, 1));
                biomeMap->set(x, y, 2, reliefMap->get(x, y, 2));

                if (isTheSameColor(x, y, reliefMap, ReliefMap::plain_color)) {
                    if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 >= high_humidity && (double)climateMap->get(x, y, 1)/255 < 0.95) {
                        biomeMap->setColor(x,y,desert_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_plain_color);
                    }
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::hills_color)) {
                    if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,desert_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_hills_color);
                    }
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::plateau_color)) {
                    if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,desert_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > hot_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < cold_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_plateau_color);
                    }
                }

            }
        }

        return biomeMap;
    }

    static ImageBase* generateBinaryBiomeMap(ImageBase* biomeMap) {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* binaryBiomeMap = new ImageBase(width, height, false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            { 
                if (isTheSameColor(x, y, biomeMap, desert_plain_color) || isTheSameColor(x, y, biomeMap, desert_hills_color) || isTheSameColor(x, y, biomeMap, desert_plateau_color) || 
                isTheSameColor(x, y, biomeMap, jungle_plain_color) || isTheSameColor(x, y, biomeMap, jungle_hills_color) || isTheSameColor(x, y, biomeMap, jungle_plateau_color) || 
                isTheSameColor(x, y, biomeMap, snow_plain_color) || isTheSameColor(x, y, biomeMap, snow_hills_color) || isTheSameColor(x, y, biomeMap, snow_plateau_color) ||
                isTheSameColor(x, y, biomeMap, ice_plain_color) || isTheSameColor(x, y, biomeMap, ice_hills_color) || isTheSameColor(x, y, biomeMap, ice_plateau_color)) {
                    binaryBiomeMap->set(x, y, 0, 255);
                }
                else {
                    binaryBiomeMap->set(x, y, 0, 0);
                }
            }
        }

        return binaryBiomeMap;
    }

    static ImageBase* generaterBinaryBorders(ImageBase* binaryBiomeMap, ImageBase* biomeMap) {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* testBiomeMap = new ImageBase(width, height, false);

        float moyR, moyG, moyB;

        int distance;

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            { 
                testBiomeMap->set(x, y, 0, 0);

                for (int i = -20; i <= 20; i++) {
                    for (int j = -20; j <= 20; j++) {
                        if (binaryBiomeMap->get(x+i, y+j, 0) == 255 && isAReliefMapColor(x, y, biomeMap)) {
                            testBiomeMap->set(x, y, 0, 255);
                        }
                    }
                }
            }
        }

        return testBiomeMap;
    }

    static float distFromBasePixel(int x, int y, int i, int j, ImageBase* image) {
        return sqrt(pow(x-(x+i), 2) + pow(y-(y+j), 2));
    }

    static vector<float> getMinDist(int x, int y, int imin, int imax, int jmin, int jmax, int height, int width, ImageBase* image, ImageBase* biomeMap) {
        float minX = -1;
        float minY = -1;
        float min = FLT_MAX;
        float dist;

        for (int i = imin; i <= imax; i++) {
            for (int j = jmin; j <= jmax; j++) {
                if (image->get(x+i, y+j, 0) == 0 && x+i > 0 && x+i < height && y+j > 0 && y+j < width) {
                    dist = distFromBasePixel(x, y, i, j, image);

                    if (dist < min && dist != 0) {
                        min = dist;
                        minX = x+i;
                        minY = y+j;
                    }
                }
            }
        }

        vector<float> minVect = {minX, minY, min};

        return minVect;
    }

    static ImageBase* smoothBordersBiome(ImageBase* reliefMap, ImageBase* climateMap, bool useDefaults) {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* smoothBordersBiomeMap = new ImageBase(width, height, true);

        ImageBase* biomeMap = BiomeMap::generateBiomeMap(reliefMap, climateMap, useDefaults);
        ImageBase* binaryBorders = BiomeMap::generaterBinaryBorders(BiomeMap::generateBinaryBiomeMap(biomeMap), biomeMap);

        float moyR, moyG, moyB;

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            { 
                if(binaryBorders->get(x, y, 0) == 255) {
                    std::vector<float> min = getMinDist(x, y, -15, 15, -15, 15, height, width, binaryBorders, biomeMap);

                    if (isABiomeMapColor(min[0], min[1], biomeMap) || isAReliefMapColor(min[0], min[1], biomeMap)) {
                        if(min[0] == -1 && min[1] == -1) {
                            moyR = biomeMap->get(x, y, 0);
                            moyG = biomeMap->get(x, y, 1);
                            moyB = biomeMap->get(x, y, 2);
                        }
                        else {
                            moyR = (min[2]*biomeMap->get(x, y, 0) + 4*biomeMap->get(min[0], min[1], 0))/(4+min[2]);
                            moyG = (min[2]*biomeMap->get(x, y, 1) + 4*biomeMap->get(min[0], min[1], 1))/(4+min[2]);
                            moyB = (min[2]*biomeMap->get(x, y, 2) + 4*biomeMap->get(min[0], min[1], 2))/(4+min[2]);
                        }

                        smoothBordersBiomeMap->set(x, y, 0, moyR);
                        smoothBordersBiomeMap->set(x, y, 1, moyG);
                        smoothBordersBiomeMap->set(x, y, 2, moyB);
                    }
                    else {
                        smoothBordersBiomeMap->set(x, y, 0, biomeMap->get(x, y, 0));
                        smoothBordersBiomeMap->set(x, y, 1, biomeMap->get(x, y, 1));
                        smoothBordersBiomeMap->set(x, y, 2, biomeMap->get(x, y, 2));
                    }

                }
                else {
                    smoothBordersBiomeMap->set(x, y, 0, biomeMap->get(x, y, 0));
                    smoothBordersBiomeMap->set(x, y, 1, biomeMap->get(x, y, 1));
                    smoothBordersBiomeMap->set(x, y, 2, biomeMap->get(x, y, 2));
                }
            }
        }

        return smoothBordersBiomeMap;
    }
};

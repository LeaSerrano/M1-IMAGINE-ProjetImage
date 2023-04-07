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

        if (!isTheSameColor(x, y, image, ReliefMap::sea_color) && !isTheSameColor(x, y, image, ReliefMap::shore_color) && !isTheSameColor(x, y, image, ReliefMap::river_color) && !isTheSameColor(x, y, image, ReliefMap::beach_color) && !isTheSameColor(x, y, image, ReliefMap::plain_color) && !isTheSameColor(x, y, image, ReliefMap::hills_color) && !isTheSameColor(x, y, image, ReliefMap::plateau_color) && !isTheSameColor(x, y, image, ReliefMap::peaks_color) && !isTheSameColor(x, y, image, ReliefMap::cliff_color)) {
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

    static ImageBase* generatePlainBiomeMap(ImageBase* reliefMap, ImageBase* climateMap,bool useDefaults) {

        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        float desert_temperature = Utilities::quantile(climateMap,0.7) / 255.0;
        if(!useDefaults) desert_temperature = DataManager::instance->requestValue("desert_temperature",desert_temperature,true);
        else DataManager::instance->setValue("desert_temperature",desert_temperature);

        float ice_temperature = Utilities::quantile(climateMap,0.3) / 255.0;
        if(!useDefaults)ice_temperature = DataManager::instance->requestValue("ice_temperature",ice_temperature,true);
        else DataManager::instance->setValue("ice_temperature",ice_temperature);

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
                    if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,desert_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_plain_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_plain_color);
                    }
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::hills_color)) {
                    if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,desert_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_hills_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_hills_color);
                    }

                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::plateau_color)) {
                    if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,desert_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 > desert_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,jungle_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 > high_humidity) {
                        biomeMap->setColor(x,y,snow_plateau_color);
                    }
                    else if ((double)climateMap->get(x, y, 0)/255 < ice_temperature && (double)climateMap->get(x, y, 1)/255 < low_humidity) {
                        biomeMap->setColor(x,y,ice_plateau_color);
                    }
                }

            }
        }

        return biomeMap;
    }
};

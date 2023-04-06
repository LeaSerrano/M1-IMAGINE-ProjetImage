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

    inline static Color desert_color = Color(255,255,50);;
    inline static Color desert_hills_color = Color(255,200,50);;
    inline static Color desert_plateau_color = Color(150,100,80);;

    inline static Color ice_color = Color(250,250,255);;
    inline static Color ice_hills_color = Color(188, 188, 209);;
    inline static Color ice_plateau_color = Color(130, 130, 173);;

    static bool isTheSameColor(int x, int y, ImageBase* image, Color color) {
        if (image->get(x, y, 0) == color.r && image->get(x, y, 1) == color.g && image->get(x, y, 2) == color.b) {
            return true;
        }
        return false;
    }

    static ImageBase* generatePlainBiomeMap(ImageBase* reliefMap, ImageBase* climateMap) {

        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        float desert_temperature = Utilities::quantile(climateMap,0.7);
        desert_temperature = DataManager::instance->requestValue("desert_temperature",desert_temperature,true);

        float ice_temperature = Utilities::quantile(climateMap,0.7);
        ice_temperature = DataManager::instance->requestValue("ice_temperature",ice_temperature,true);

        float humidity = DataManager::instance->requestValue("humidity");


        ImageBase* biomeMap = new ImageBase(width, height, true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            { 
                biomeMap->set(x, y, 0, reliefMap->get(x, y, 0));
                biomeMap->set(x, y, 1, reliefMap->get(x, y, 1));
                biomeMap->set(x, y, 2, reliefMap->get(x, y, 2));

                if (isTheSameColor(x, y, reliefMap, ReliefMap::plain_color) && (double)climateMap->get(x, y, 0)/255 > desert_temperature) {
                    biomeMap->setColor(x,y,desert_color);
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::hills_color) && (double)climateMap->get(x, y, 0)/255 > desert_temperature) {
                    biomeMap->setColor(x,y,desert_hills_color);
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::plateau_color) && (double)climateMap->get(x, y, 0)/255 > desert_temperature) {
                    biomeMap->setColor(x,y,desert_plateau_color);
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::plain_color) && (double)climateMap->get(x, y, 0)/255 < ice_temperature) {
                    biomeMap->setColor(x,y,ice_color);
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::hills_color) && (double)climateMap->get(x, y, 0)/255 < ice_temperature) {
                    biomeMap->setColor(x,y,ice_hills_color);
                }
                else if (isTheSameColor(x, y, reliefMap, ReliefMap::plateau_color) && (double)climateMap->get(x, y, 0)/255 < ice_temperature) {
                    biomeMap->setColor(x,y,ice_plateau_color);
                }

            }
        }

        return biomeMap;
    }
};
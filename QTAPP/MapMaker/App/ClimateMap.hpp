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



class ClimateMap
{

public:

    static ImageBase* generateTemperature() {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* perlin = Noise::generatePerlin(width,height,scale/2,1);

        ImageBase* image = new ImageBase(width,height,true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {   
                image->set(x,y,0, perlin->get(x, y, 0));
                image->set(x,y,1, 0);
                image->set(x,y,2, 0);
            }
        }

        return image;
    }

    static ImageBase* generateHumidity() {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* perlin = Noise::generatePerlin(width,height,scale/2,1);

        ImageBase* image = new ImageBase(width,height,true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {   
                image->set(x,y,0, 0);
                image->set(x,y,1, perlin->get(x, y, 0));
                image->set(x,y,2, 0);
            }
        }

        return image;
    }

    static ImageBase* generateClimateMap() {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* temperature = generateTemperature();
        ImageBase* humidity = generateHumidity();

        ImageBase* image = new ImageBase(width,height,true);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {   
                image->set(x,y,0, temperature->get(x, y, 0));
                image->set(x,y,1, humidity->get(x, y, 1));
                image->set(x,y,2, 0);
            }
        }

        return image;
    }

};
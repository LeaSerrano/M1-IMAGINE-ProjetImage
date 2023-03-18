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


class ReliefMap
{

public:

    inline static Color sea_color;     //Main sea
    inline static Color shore_color;   //Not deep sea
    inline static Color beach_color;   //Beach level, small gradient
    inline static Color plain_color;   //Plain and hills level, small gradient
    inline static Color hills_color;   //Plain and Hills level, medium gradient
    inline static Color cliff_color;   //Above sea level, high gradient
    inline static Color plateau_color; //Plateau and peak level, small gradient
    inline static Color peaks_color;   //Plateau and peak level, high gradient

    static void setup()
    {
        sea_color = Color(30,30,255);
        shore_color = Color(100,100,255);
        beach_color = Color(200,200,50);
        plain_color = Color(100,255,100);
        hills_color = Color(150,100,80);
        cliff_color = Color(50,50,50);
        plateau_color = Color(100,100,80);
        peaks_color = Color(200,200,200);
    }

    static ImageBase* reliefMap(ImageBase* gradientMap, ImageBase* heightMap)
    {
        setup();

        ImageBase* map = new ImageBase(gradientMap->getWidth(),gradientMap->getHeight(),true);

        float sea_level = DataManager::instance->requestValue("sea_level");
        float shore_level = DataManager::instance->requestValue("shore_level");
        float beach_level = DataManager::instance->requestValue("beach_level");
        float plain_level = DataManager::instance->requestValue("plain_level");
        float hills_level = DataManager::instance->requestValue("hills_level");
        float plateau_level = DataManager::instance->requestValue("plateau_level");

        Color color;
        for(int x = 0; x < gradientMap->getWidth(); x++)
        {
            for(int y = 0; y < gradientMap->getHeight(); y++)
            {
                float altitude = (float)(heightMap->get(x, y, 0)) / 255.0;

                if(altitude < sea_level)
                {
                    altitude /= sea_level;
                    if(altitude < 1.0-shore_level)
                    {
                        color = sea_color;
                    }
                    else
                    {
                        color = shore_color;
                    }
                }
                else
                {
                    altitude -= sea_level;
                    altitude /= (1.0-sea_level);
                    if(altitude < beach_level)
                    {
                        color = beach_color;
                    }
                    else if(altitude < plain_level)
                    {
                        color = plain_color;
                    }
                    else if(altitude < hills_level)
                    {
                        color = plain_color;
                    }
                    else if(altitude < plateau_level)
                    {
                        color = plateau_color;
                    }
                    else
                    {
                        color = peaks_color;
                    }
                }

                map->setColor(x,y,color);
            }
        }

        return map;
    }


};

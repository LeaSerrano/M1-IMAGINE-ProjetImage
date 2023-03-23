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

    inline static Color sea_color = Color(30,30,255);;     //Main sea
    inline static Color shore_color = Color(30,80,255);   //Not deep sea

    inline static Color river_color = Color(30,150,255);;   //Plateau and peak level, high gradient

    inline static Color beach_color = Color(200,200,50);;   //Beach level, small gradient
    inline static Color plain_color = Color(100,255,100);;   //Plain and hills level, small gradient
    inline static Color hills_color = Color(80,200,80);;   //Plain and Hills level, medium gradient
    inline static Color plateau_color = Color(100,100,80);; //Plateau and peak level, small gradient
    inline static Color peaks_color = Color(200,200,200);;   //Plateau and peak level, high gradient

    inline static Color cliff_color = Color(50,50,50);;   //Above sea level, high gradient

    inline static double hills_gradient = 0.4;
    inline static double cliff_gradient = 0.8;

    /*static void setup()
    {
        sea_color = Color(30,30,255);
        shore_color = Color(100,100,255);
        beach_color = Color(200,200,50);
        plain_color = Color(100,255,100);
        hills_color = Color(80,200,80);
        cliff_color = Color(50,50,50);
        plateau_color = Color(100,100,80);
        peaks_color = Color(200,200,200);
    }*/

    static ImageBase* reliefMap(ImageBase* gradientMap, ImageBase* heightMap,ImageBase* seaBinary,ImageBase* riverMap)
    {
        //setup();

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
                float gradient = (float)(gradientMap->get(x,y,2)) / 255.0;

                if(seaBinary->get(x,y,0) <= 0)
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
                else if(riverMap->get(x,y,0) > 0)
                {
                    color = river_color;
                }
                else
                {
                    altitude -= sea_level;
                    altitude /= (1.0-sea_level);
                    if(altitude < beach_level)
                    {
                        color = beach_color;
                    }/*
                    else if(altitude < plain_level )
                    {
                        color = plain_color;
                    }*/
                    else if(altitude < hills_level) //and plains
                    {
                        if(gradient > cliff_gradient)
                        {
                            color = cliff_color;
                        }
                        else if(gradient > hills_gradient)
                        {
                            color = hills_color;
                        }
                        else
                        {
                            color = plain_color;
                        }
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

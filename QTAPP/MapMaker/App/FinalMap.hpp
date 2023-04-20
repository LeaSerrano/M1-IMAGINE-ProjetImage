#pragma once

#include "../Library/ImageBase.h"
#include "DataManager.hpp"
#include "BiomeMap.hpp"
#include "PointOfInterestMap.hpp"

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


class FinalMap
{

public:

    inline static Color city_color = Color(20,20,20);
    inline static Color forest_color = Color(25,80,25);

    static ImageBase* generate(ImageBase* biomeMap, ImageBase* poiMap)
    {
        ImageBase* map = new ImageBase(biomeMap->getWidth(),biomeMap->getHeight(),true);

        vector<InterestPoint*> interestPoints = PointOfInterestMap::generatePointsList();

        for(int x = 0; x < map->getWidth();x++)
        {
            for(int y = 0; y < map->getHeight();y++)
            {
                bool iped = false;
                for(InterestPoint* ip : interestPoints)
                {
                    if(poiMap->get(x,y,0) == ip->color.r && poiMap->get(x,y,1) == ip->color.g && poiMap->get(x,y,2) == ip->color.b)
                    {
                        if(ip->stringId == "city")
                        {
                            map->setColor(x,y,city_color); iped = true;
                        }
                        else if(ip->stringId == "forest")
                        {
                            map->setColor(x,y,forest_color); iped = true;
                        }
                        break;
                    }
                }
                if(!iped)
                {
                    map->setColor(x,y,biomeMap->readColor(x,y));
                }
            }
        }

        return map;
    }
};

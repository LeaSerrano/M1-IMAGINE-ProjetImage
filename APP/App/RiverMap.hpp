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


#define max_river_depth 2

#define river_split_min_lenght 50 //Min lenght needed before split , in river_width    river_width*this
#define river_split_after_lenght 3 //Lenght river tarvels after split before starting following gradient
#define river_split_gradient 0.1
#define river_split_angle 0.785//45°

#define river_end_gradient 0.02
#define river_min_proximity 2 //In river_width

class RiverMap
{

public:

    static void selectRiver(int& riverStartX, int& riverStartY, ImageBase* allRivers, ImageBase* seaBinary)
    {
        double randX = (rand() / (double)RAND_MAX); randX = max(0.05,min(0.9,randX)); //Select an x between 10% and 70% of the map width
        double randY = (rand() / (double)RAND_MAX); randY = max(0.05,min(0.95,randY)); //Select an Y between 10% and 90% of the map height

        riverStartX = (int)(randX * seaBinary->getWidth()); // (int)(0.45 * seaBinary->getWidth());//
        riverStartY = (int)(randY * seaBinary->getHeight()); // (int)(0.45 * seaBinary->getHeight());//

        bool ok = false;
        do
        {
            for(; riverStartX < seaBinary->getWidth() - 1; riverStartX++)
            {
                if(allRivers->get(riverStartX,riverStartY,0) > 0){continue;}
                if(seaBinary->get(riverStartX,riverStartY,0) != seaBinary->get(riverStartX + 1,riverStartY,0) )
                {
                    ok = true; break;
                }
            }
            if(!ok)
            {
                riverStartY++;
                riverStartX = (int)(randX * seaBinary->getWidth());
            }
        }while(!ok);
    }

    static void drawRound(int x, int y, float radius, ImageBase* riverMap)
    {
        int r = (int)radius;
        for(int dx = -r; dx <= r; dx++)
        {
            for(int dy = -r; dy <= r; dy++)
            {
                if(x + dx < 0 || x + dx >= riverMap->getWidth() || y + dy < 0 || y + dy >= riverMap->getHeight()){continue;}

                float d = sqrt((dx*dx) + (dy*dy));
                if(d <= radius)
                {
                    riverMap->set(x + dx,y + dy,0,255);
                }
            }
        }
    }

    static int drawRiver(int startX, int startY, ImageBase* allRivers, ImageBase* riverMap, double river_width, ImageBase* gradientMap,ImageBase* heightMap, int depth, double dirX, double dirY)
    {
        int lenght = 0;
        int x = startX; int y = startY;
        double hills_level = DataManager::instance->getValue("hills_level");

        for(; lenght < 5000; lenght++)
        {
            drawRound(x,y,river_width,riverMap);

            double altitude = heightMap->get(x,y,0) / 255.0;

            double gradientX = -(gradientMap->get(x,y,0)-128) / 128.0;
            double gradientY = -(gradientMap->get(x,y,1)-128) / 128.0;
            double gradientNorm = gradientMap->get(x,y,2) / 255.0;
            //Normalisation du gradient
            //gradientX /= gradientNorm;
            //gradientY /= gradientNorm;
            //Extension pour avoir x et y >= 1
            double m = max(1.0/river_width,min(abs(gradientX),abs(gradientY)));
            gradientX /= m;
            gradientY /= m;

            if(lenght < (river_split_after_lenght*river_width) && dirX != 0 && dirY != 0) //
            {
                x += dirX; y += dirY;
            }
            else
            {
                if(gradientNorm < river_end_gradient && depth >= max_river_depth)
                {
                    break;  //Reached maximum depth
                }
                //Separate the river into two smaller affluents if we reached a flat zone or an altitude of plateau
                else if(
                    (gradientNorm < river_split_gradient || altitude >= hills_level) &&
                    lenght > (river_split_min_lenght*river_width) &&
                    depth < max_river_depth)
                {
                    if(x < 0 || x >= riverMap->getWidth() || y < 0 || y >= riverMap->getHeight()){break;}

                    double dirX1 = cos(river_split_angle)*gradientX - sin(river_split_angle)*gradientY;
                    double dirY1 = sin(river_split_angle)*gradientX + cos(river_split_angle)*gradientY;

                    double dirX2 = cos(-river_split_angle)*gradientX - sin(-river_split_angle)*gradientY;
                    double dirY2 = sin(-river_split_angle)*gradientX + cos(-river_split_angle)*gradientY;

                    int l1 = drawRiver(x,y, allRivers,riverMap,river_width / 2.0, gradientMap,heightMap,depth+1,dirX1,dirY1);
                    int l2 = drawRiver(x,y, allRivers,riverMap,river_width / 2.0, gradientMap,heightMap,depth+1,dirX2,dirY2);

                    lenght += l1 + l2;
                    break;
                }
            }

            x += (int)(gradientX * river_width * 0.25); y += (int)(gradientY * river_width * 0.25);

            if(x < 0 || x >= riverMap->getWidth() || y < 0 || y >= riverMap->getHeight()){break;}   //Finish if we go out of bounds

            if(allRivers->get(x,y,0) > 0){return lenght;}     //Abort this river if we reached an already existing river
        }

        return lenght;
    }

    static int drawRiver(ImageBase* allRivers, ImageBase*& riverMap, double river_width, ImageBase* gradientMap,ImageBase* heightMap, ImageBase* seaBinary)
    {
        bool abort = false; int lenght = 0;
        do
        {
            int startX,startY; abort = false;
            selectRiver(startX,startY,allRivers,seaBinary);

            riverMap = new ImageBase(allRivers->getWidth(),allRivers->getHeight(),false);
            lenght = drawRiver(startX,startY,allRivers,riverMap,river_width,gradientMap,heightMap,0,0,0);

            if(lenght < 0) {abort = true;}
        } while(abort);

        //Draw this river to the allrivers map
        int r = (int)(river_width*river_min_proximity);
        for(int x = 0; x < riverMap->getWidth(); x++)
        {
            for(int y = 0; y < riverMap->getHeight(); y++)
            {
                if(allRivers->get(x,y,0) <= 0 && riverMap->get(x,y,0) > 0)
                {
                    bool has = false;
                    for(int dx = -r; dx <= r; dx++)
                    {
                        for(int dy = -r; dy <= r; dy++)
                        {
                            if(x + dx < 0 || x + dx >= riverMap->getWidth() || y + dy < 0 || y + dy >= riverMap->getHeight()){continue;}

                            float d = sqrt((dx*dx) + (dy*dy));
                            if(d <= river_width);
                            {
                                allRivers->set(x,y,255);
                            }
                        }
                    }
                }
            }
        }

        return lenght;
    }

   
    static ImageBase* riverMap(ImageBase* gradientMap, ImageBase* heightMap, ImageBase* seaBinary)
    {
        ImageBase* map = new ImageBase(gradientMap->getWidth(),gradientMap->getHeight(),false);

        double mapKm =  DataManager::instance->requestValue("map_scale") ;
        double mapSize = DataManager::instance->requestValue("map_size");
        double kmPerPixel = mapKm / mapSize;

        double river_width = (0.1 / kmPerPixel) * 4; //Will return 4pix for a 200km map with 2048pix resolution          //gradientMap->getWidth() / 512.0; // << width of 4pix or a 2048 map

        int min_river_lenght = (0.1 / kmPerPixel) * 30; //Will return 30 pix for a 200km map with 2048pix resolution
        int river_count = (int)( (mapKm / 200.0) * 15); //Will return 15 rivers for a 200km map with 2048pix resolution
        int river_try_count = 150;

        vector<ImageBase*> maps;
        vector<int> lenghts;

        ImageBase* combined = new ImageBase(map->getWidth(),map->getHeight(),false);
        for(int i = 0; i < river_try_count; i++)
        {
            ImageBase* m = new ImageBase(map->getWidth(),map->getHeight(),false);

            int l = drawRiver(combined,m,river_width,gradientMap,heightMap,seaBinary);

            if(l >= min_river_lenght)
            {
                maps.push_back(m);
                lenghts.push_back(l);
            }
        }

        vector<ImageBase*> ordered_maps;

        for(int m = 0; m < maps.size();m++)
        {
            int maxId = 0;
            for(int i = 1; i < maps.size(); i++)
            {
                if(lenghts.at(i) > lenghts.at(maxId))  { maxId = i;  }
            }
            ordered_maps.push_back(maps.at(maxId));
            maps.erase(maps.begin() + maxId);
            lenghts.erase(lenghts.begin() + maxId);
            m = 0;
        }

        for(int x = 0; x < map->getWidth(); x++)
        {
            for(int y = 0; y < map->getHeight(); y++)
            {
                for(int i = 0; i < river_count && i < ordered_maps.size(); i++)
                {
                    if(map->get(x,y,0) <= 0)
                    {
                        map->set(x,y,0,ordered_maps.at(i)->get(x,y,0));
                    }
                }
            }
        }

        return map;
    }


};

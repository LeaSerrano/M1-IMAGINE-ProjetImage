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


#define max_river_depth 3
#define river_split_lenght 1000


class RiverMap
{

public:

    static void selectRiver(int& riverStartX, int& riverStartY, ImageBase* allRivers, ImageBase* seaBinary)
    {
        double randX = (rand() / (double)RAND_MAX); randX = max(0.1,min(0.7,randX)); //Select an x between 10% and 70% of the map width
        double randY = (rand() / (double)RAND_MAX); randY = max(0.1,min(0.9,randY)); //Select an Y between 10% and 90% of the map height

        riverStartX = (int)(randX * seaBinary->getWidth());
        riverStartY = (int)(randY * seaBinary->getHeight());

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

    static int drawRiver(int startX, int startY, ImageBase* allRivers, ImageBase* riverMap, double river_width, ImageBase* gradientMap, int depth, int divider)
    {
        int lenght = 0;
        int x = startX; int y = startY;

        for(; lenght < 3000; lenght++)
        {
            drawRound(x,y,river_width,riverMap);

            double gradientX = -(gradientMap->get(x,y,0)-128) / 128.0; 
            double gradientY = -(gradientMap->get(x,y,1)-128) / 128.0;
            if(lenght < river_split_lenght && divider != 0)
            {         
                if(abs(gradientX) > abs(gradientY))
                {
                    gradientX /= abs(gradientX);
                    gradientY /= abs(gradientY);
                    gradientY *= 2 * divider;
                }
                else
                {
                    gradientX /= abs(gradientX);
                    gradientY /= abs(gradientY);
                    gradientX *= 2 * divider;
                }
            }
            else
            {
                gradientX *= 4;
                gradientY *= 4;
            }     

            if(gradientX < 1 && gradientY < 1) //Separate the river into two smaller affluents if we reached a flat zone
            {
                if(depth > max_river_depth){break;}

                x += (gradientX / abs(gradientX))*river_width;
                y += (gradientY / abs(gradientY))*river_width;

                if(x < 0 || x >= riverMap->getWidth() || y < 0 || y >= riverMap->getHeight()){break;}

                int l1 = drawRiver(x,y, allRivers,riverMap,river_width / 2.0, gradientMap,depth+1,1);
                int l2 = drawRiver(x,y, allRivers,riverMap,river_width / 2.0, gradientMap,depth+1,-1);
                lenght += max(l1,l2);
                break;             
            }

            x += gradientX; y += gradientY;

            if(x < 0 || x >= riverMap->getWidth() || y < 0 || y >= riverMap->getHeight()){break;}   //FInish if we go out of bounds

            if(allRivers->get(x,y,0) > 0){return -1;}     //Abort this river if we reached an already existing river
        }

        return lenght;
    }

    static int drawRiver(ImageBase* allRivers, ImageBase* riverMap, double river_width, ImageBase* gradientMap, ImageBase* seaBinary)
    {
        bool abort = false; int lenght = 0;
        do
        {
            int startX,startY; abort = false;
            selectRiver(startX,startY,allRivers,seaBinary);

            lenght = drawRiver(startX,startY,allRivers,riverMap,river_width,gradientMap,0,0);
            if(lenght < 0) {abort = true;}

            /*int x = startX; int y = startY;
            for(; lenght < 3000; lenght++)
            {
                drawRound(x,y,river_width,riverMap);

                double gradientX = -(gradientMap->get(x,y,0)-128) / 128.0;
                gradientX *= 4;
                double gradientY = -(gradientMap->get(x,y,1)-128) / 128.0;
                gradientY *= 4;

                if(gradientX < 1 && gradientY < 1){break;}  //Finish if we reach a flat zone

                x += gradientX; y += gradientY;

                if(x < 0 || x >= riverMap->getWidth() || y < 0 || y >= riverMap->getHeight()){break;}   //FInish if we go out of bounds

                if(allRivers->get(x,y,0) > 0){abort = true; break;}     //Start over if we reached an already existing river
            }*/
        } while(abort);

        //Draw this river to the allrivers map
        for(int i = 0; i < riverMap->getSize();i++)
        {
            if(allRivers->get(i,0) <= 0)
            {
                allRivers->set(i,0,riverMap->get(i,0));
            }
        }

        return lenght;
    }

   
    static ImageBase* riverMap(ImageBase* gradientMap, ImageBase* seaBinary)
    {
        ImageBase* map = new ImageBase(gradientMap->getWidth(),gradientMap->getHeight(),false);

        /*double kmPerPixel = DataManager::instance->requestValue("map_scale") / DataManager::instance->requestValue("map_size");
        cout << "kmPerPixel : " << kmPerPixel << endl;*/
        double river_width = 5; int min_river_lenght = 30;//DataManager::instance->requestValue("river_width");
        int river_count = 10;
        int river_try_count = 150;

        vector<ImageBase*> maps;
        vector<int> lenghts;

        ImageBase* combined = new ImageBase(map->getWidth(),map->getHeight(),false);
        for(int i = 0; i < river_try_count; i++)
        {
            ImageBase* m = new ImageBase(map->getWidth(),map->getHeight(),false);
            int l = drawRiver(combined,m,river_width,gradientMap,seaBinary);
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

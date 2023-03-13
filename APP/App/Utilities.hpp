#pragma once

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>

using namespace std;
using namespace siv;

class Utilities
{
public:

    static ImageBase* remap(ImageBase* img, int min, int max)
    {
        ImageBase* map = new ImageBase(img->getWidth(),img->getHeight(),false);

        int imax = 0; int imin = 0;
        for(int i = 0; i < img->getSize();i++)
        {
            if(img->get(i,0) > imax) {imax = img->get(i,0);}
            if(img->get(i,0) < imin) {imin = img->get(i,0);}
        }

        for(int i = 0; i < img->getSize();i++)
        {
            float v = img->get(i,0);
            v -= imin;
            v /= (float)(imax-imin);

            v *= (max-min);
            v += min;
            map->set(i,0,v);
        }

        return map;
    }

    static int quantile(ImageBase* img, double ratio)
    {
        vector<int> values;
        for(int i = 0; i < img->getSize();i++)
        {
            values.push_back(img->get(i,0));
        }

        sort(values.begin(), values.end());

        return values.at((values.size()-1) * ratio);
    }


};

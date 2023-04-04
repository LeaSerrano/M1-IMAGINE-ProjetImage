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

struct Point {
    int x;
    int y;
};

struct Couleur {
    int r;
    int g;
    int b;

    bool operator==(const Couleur& other) const {
        return (r == other.r && g == other.g && b == other.b);
    }
};


class PointOfInterestMap
{

public:

    const static int waterfallValue = 128;
    const static int harborValue = 255;

    static ImageBase* waterfallDetection(int y, int x, ImageBase* imageGradient, ImageBase* riverMap, ImageBase* image) {
        if (imageGradient->get(x, y, 2) > 150 && riverMap->get(x, y, 0) > 0 && riverMap->get(x, y, 1) > 0 && riverMap->get(x, y, 2) > 0)  {
            image->set(x,y,0, waterfallValue);
        }
        return image;
    }

    static ImageBase* harborDetection(int y, int x, ImageBase* seaBinary, ImageBase* riverMap, ImageBase* image, int size) {
        if(seaBinary->get(x,y,0) <= 0){
            if (riverMap->get(x, y, 0) > 0 && riverMap->get(x, y, 1) > 0 && riverMap->get(x, y, 2) > 0) { 
                image->set(x,y,0, harborValue);
            }
        }

        return image;
    }


    static ImageBase* generatePointOfInterestMapGray(ImageBase* imageGradient, ImageBase* riverMap, ImageBase* seaBinary, ImageBase* imageRelief, ImageBase* HeightMap)
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* image = new ImageBase(width,height,false);

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {  
                image->set(x,y,0, 0);

                image = waterfallDetection(y, x, imageGradient, riverMap, image);
                image = harborDetection(y, x, seaBinary, riverMap, image, width);
            }
        }


        return image;
    }

static int getLabelValue(map<int, vector<Point>> labels, Point value) {
    for (auto const& [key, val] : labels) {
        for (auto const& point : val) {
            if (point.x == value.x && point.y == value.y) {
                return key;
            }
        }
    }
    return -1;
}

    static map<int, vector<Point>> distanceSegmentation(ImageBase* image, int width, int height) {
        int i, j, k;
        map<int, vector<Point>> labels;

        int label = -1;
        double d;
        int labelValue;
        int test = -1;

        double mapKm =  DataManager::instance->requestValue("map_scale") ;
        double mapSize = DataManager::instance->requestValue("map_size");
        double kmPerPixel = mapKm / mapSize;

        int radius = (int)(2.0/kmPerPixel); //For kmPIxel=0.1 >> 20 pixels radius

        for(int y1 = 0; y1 < height ; y1++)
        {
            for(int x1 = 0; x1 < width; x1++)
            {  
                test = -1;

                if ((int)image->get(x1, y1, 0) != 0) {

                    for (int y2 = y1 - radius; y2 <= y1 + radius; y2++) {
                        for (int x2 = x1 - radius; x2 <= x1 + radius; x2++) {
                            if ((int)image->get(x2, y2, 0) != 0 && (int)image->get(x2, y2, 0) == (int)image->get(x1, y1, 0) && test == -1) {
                                labelValue = getLabelValue(labels,{x2, y2});

                                if (labelValue != -1) {
                                    labels[labelValue].push_back({x1, y1});
                                    test = 0;
                                }
                            }
                        }
                    }

                    if (test == -1) {
                        label++;
                        labels[label].push_back({x1, y1});
                    }

                }
            }
        }

        return labels;

    }

    static map<int, vector<Point>> correctLabelError(map<int, vector<Point>> labels, ImageBase* image)
    {
        double distance_min = 20;

        map<int, vector<Point>> labels_copy = labels;
        map<int, vector<Point>> new_labels;

        for (auto& label : labels) {
            int index = label.first;

            for (auto& point : label.second) {
                for (auto& other_label : labels_copy) {
                    if (other_label.first != index) {
                        for (auto& other_point : other_label.second) {
                            double distance = sqrt(pow(point.x - other_point.x, 2) + pow(point.y - other_point.y, 2));

                            if (distance < distance_min && (int)image->get(point.x, point.y, 0) == (int)image->get(other_point.x, other_point.y, 0)) {
                                vector<Point> new_points;
                                new_points.insert(new_points.end(), label.second.begin(), label.second.end());
                                new_points.insert(new_points.end(), other_label.second.begin(), other_label.second.end());

                                new_labels[new_labels.size()] = new_points;

                                labels_copy.erase(index);
                                labels_copy.erase(other_label.first);

                                goto end_of_loop;
                            }
                        }
                    }
                }
            }
            end_of_loop:;
        }

        for (auto& label : labels_copy) {
            new_labels[new_labels.size()] = label.second;
        }

        labels = new_labels;
        return labels;
    }



    static ImageBase* generatePointOfInterestMapColored(ImageBase* image)
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* imageColor = new ImageBase(width,height,true);


        map<int, vector<Point>> labels;
        labels = distanceSegmentation(image, width, height);

        labels = correctLabelError(labels, image);

        vector<Couleur> color;
        

        Couleur c;
        c.r = rand() % ((255 - 0) + 1) + 0;
        c.g = rand() % ((255 - 0) + 1) + 0;
        c.b = rand() % ((255 - 0) + 1) + 0;

        for (auto const& [key, val] : labels) {

            if (c.r > 255) {
                    c.r = 255;
                }
                else if (c.r < 0) {
                    c.r = 0;
                }

                if (c.g > 255) {
                    c.g = 255;
                }
                else if (c.g < 0) {
                    c.g = 0;
                }

                if (c.b > 255) {
                    c.b = 255;
                }
                else if (c.b < 0) {
                    c.b = 0;
                }


                if(std::find(color.begin(), color.end(), c) != color.end() || (c.r == 0 && c.g == 0 && c.b == 0)) {
                    while (std::find(color.begin(), color.end(), c) != color.end() || (c.r == 0 && c.g == 0 && c.b == 0)) {
                        c.r = rand() % ((255 - 0) + 1) + 0;
                        c.g = rand() % ((255 - 0) + 1) + 0;
                        c.b = rand() % ((255 - 0) + 1) + 0;
                    }
                }
                else {
                    color.push_back(c);
                }

            for (auto const& point : val) {

                imageColor->set(point.x, point.y, 0, c.r);
                imageColor->set(point.x, point.y, 1, c.g);
                imageColor->set(point.x, point.y, 2, c.b);
            }

            c.r = rand() % ((255 - 0) + 1) + 0;
            c.g = rand() % ((255 - 0) + 1) + 0;
            c.b = rand() % ((255 - 0) + 1) + 0;
        }


        return imageColor;
    }

};
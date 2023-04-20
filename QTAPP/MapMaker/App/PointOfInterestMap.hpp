#pragma once

#include "../Library/ImageBase.h"
#include "Noise.hpp"
#include "Utilities.hpp"
#include "DataManager.hpp"
#include "ProjectManager.h"
#include "BiomeMap.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <unordered_map>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>

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

    Couleur(){}
    Couleur(int _r, int _g, int _b)
    {
        r = _r; g = _g; b = _b;
    }

    bool operator==(const Couleur& other) const {
        return (r == other.r && g == other.g && b == other.b);
    }
};

struct InterestPoint
{
    int intId;
    string stringId;
    Couleur color;

    string name;

    vector<Point> points;
    int size;
    int x,y;
};

#define pointRadius 4

#define waterfallValue 128
#define harborValue 255

#define meadowValue 200
#define meadowGradient 20
#define meadowAltitudeDiff 1
#define meadowRatio 0.75

#define forest_occurence 0.5
#define city_occurence 0.5

#define lakeValue 30
#define lakeExtend 8

#define PointsFileName "interestPoints.txt"

class PointOfInterestMap
{

public:
    

    static inline unordered_map<int,string> interestBaseTypes =
    {
        {waterfallValue,"waterfall"},
        {harborValue,"harbor"},
        {meadowValue,"meadow"},
        {lakeValue,"lake"}
    };

    static inline unordered_map<string,vector<string>> interestTypes =
        {
            {"waterfall",{"waterfall"}},
            {"harbor",{"harbor","mouth"}},
            {"meadow",{"meadow","city","forest"}},
            {"lake",{"lake"}}
    };

    static inline vector<vector<string>> nameParts =
    {
            {"Da","La","Mo","Fe","Ko","Pi","Ku","He"},
            {"mi","lo","la","ma","me","le","pa","pe","po","na","ni"},
            {"da","er","pit","mot","ger","bat"}
    };


    static ImageBase* waterfallDetection(int y, int x, ImageBase* imageGradient, ImageBase* riverMap, ImageBase* image) {
        if (image->get(x,y,0) <= 0 && imageGradient->get(x, y, 2) > 150 && riverMap->get(x, y, 0) > 0 && riverMap->get(x, y, 1) > 0 && riverMap->get(x, y, 2) > 0)  {
            image->set(x,y,0, waterfallValue);
        }
        return image;
    }

    static ImageBase* harborDetection(int y, int x, ImageBase* seaBinary, ImageBase* riverMap, ImageBase* image) {
        if(image->get(x,y,0) <= 0 && seaBinary->get(x,y,0) <= 0){
            if (riverMap->get(x, y, 0) > 0 && riverMap->get(x, y, 1) > 0 && riverMap->get(x, y, 2) > 0) { 
                image->set(x,y,0, harborValue);
            }
        }

        return image;
    }

    static ImageBase* meadowDetection(int y, int x, ImageBase* seaBinary, ImageBase* imageGradient, ImageBase* heightMap, ImageBase* image, double kmPerPixel) 
    {
        if( image->get(x,y,0) <= 0 && seaBinary->get(x,y,0) > 0 && imageGradient->get(x,y,2) <= meadowGradient)
        {
            int radius = (int)(1.0/kmPerPixel); //For kmPix=0.1 >> 10 radius

            int altitude = heightMap->get(x,y,0);

            int count = 0;
            for(int dx = -radius; dx <= radius; dx++)
            {
                for(int dy = -radius; dy <= radius; dy++)
                {
                    if(x + dx < 0 || x + dx >= image->getWidth() || y + dy < 0 || y + dy >= image->getHeight()){continue;}

                    if(imageGradient->get(x+dx,y+dy,2) <= meadowGradient && abs(altitude-heightMap->get(x+dx,y+dy,0)) <= meadowAltitudeDiff)
                    {
                        count++;
                    }
                }
            }

            if((double)(count) / (double)(radius*radius) > meadowRatio)
            {
                image->set(x,y,0,meadowValue);
            }
        }

        return image;
    }

    static ImageBase* lakeDetection(int y, int x, ImageBase* riverMap, ImageBase* image, double kmPerPixel) 
    {
        if(image->get(x,y,0) <= 0 && riverMap->get(x,y,0) > 0)
        {
            int radius = (int)(1.0/kmPerPixel); //For kmPix=0.1 >> 10 radius

            int dx = 0;
            for(int i = 0; i < radius; i++)
            {
                if(x+i>=image->getWidth()){break;}
                if(riverMap->get(x+i,y,0) <= 0){break;}
                dx++;
            }
            for(int i = 0; i < radius; i++)
            {
                if(x-i < 0){break;}
                if(riverMap->get(x-i,y,0) <= 0){break;}
                dx++;
            }
            int dy = 0;
            for(int i = 0; i < radius; i++)
            {
                if(y+i>=image->getHeight()){break;}
                if(riverMap->get(x,y+i,0) <= 0){break;}
                dy++;
            }
            for(int i = 0; i < radius; i++)
            {
                if(y-i < 0){break;}
                if(riverMap->get(x,y-i,0) <= 0){break;}
                dy++;
            }

            int ext = (int)(lakeExtend*(0.1/kmPerPixel)); // << return lakeExtend*1 for 0.1kmPerPixel
            if(dx >= ext && dy >= ext)
            {
                image->set(x,y,0,lakeValue);
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

        double mapKm =  DataManager::instance->requestValue("map_scale") ;
        double mapSize = DataManager::instance->requestValue("map_size");
        double kmPerPixel = mapKm / mapSize;

        for(int y = 0; y < height ; y++)
        {
            for(int x = 0; x < width; x++)
            {  
                image->set(x,y,0, 0);

                image = lakeDetection(y,x,riverMap,image,kmPerPixel);
                image = waterfallDetection(y, x, imageGradient, riverMap, image);
                image = harborDetection(y, x, seaBinary, riverMap, image);
                image = meadowDetection(y,x,seaBinary,imageGradient,HeightMap,image,kmPerPixel);
            }
        }


        return image;
    }

    static int getLabelValue(unordered_map<int, vector<Point>> labels, Point value) {
        for (auto const& [key, val] : labels) {
            for (auto const& point : val) {
                if (point.x == value.x && point.y == value.y) {
                    return key;
                }
            }
        }
        return -1;
    }

    static unordered_map<int, vector<Point>> distanceSegmentation(ImageBase* image, int width, int height) {
        int i, j, k;
        unordered_map<int, vector<Point>> labels;

        int label = -1;
        double d;
        int labelValue;
        int test = -1;

        double mapKm =  DataManager::instance->requestValue("map_scale") ;
        double mapSize = DataManager::instance->requestValue("map_size");
        double kmPerPixel = mapKm / mapSize;

        int radius = (int)(2.0/kmPerPixel); //For kmPIxel=0.1 >> 20 pixels radius

        Point point;
        for(int y1 = 0; y1 < height ; y1++)
        {
            for(int x1 = 0; x1 < width; x1++)
            {  
                test = -1;

                int v1 = image->get(x1, y1, 0);
                if (v1 != 0) {

                    for (int y2 = y1 - radius; y2 <= y1 + radius && test == -1; y2++) 
                    {
                        for (int x2 = x1 - radius; x2 <= x1 + radius && test == -1; x2++) 
                        {
                            if (image->get(x2, y2, 0) == v1) 
                            {
                                point.x = x2; point.y = y2;
                                labelValue = getLabelValue(labels,point);

                                if (labelValue != -1) {
                                    point.x = x1; point.y = y1;
                                    labels[labelValue].push_back(point);
                                    test = 0;
                                }
                            }
                        }
                    }

                    if (test == -1) {
                        label++;
                        point.x = x1; point.y = y1;
                        labels[label].push_back(point);
                    }

                }
            }
        }

        return labels;

    }

    static unordered_map<int, vector<Point>> correctLabelError(unordered_map<int, vector<Point>> labels, ImageBase* image)
    {
        double distance_min = 20;

        unordered_map<int, vector<Point>> labels_copy = labels;
        unordered_map<int, vector<Point>> new_labels;

        double mapKm =  DataManager::instance->requestValue("map_scale") ;
        double mapSize = DataManager::instance->requestValue("map_size");
        double kmPerPixel = mapKm / mapSize;
        int zoneSize = (int)(3/kmPerPixel); //30 for 0.1kmPix

        for (auto& label : labels) {
            int index = label.first;
            if(label.second.size() > zoneSize){continue;}

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

    static ImageBase* generatePointOfInterestMapColored(ImageBase* image,ImageBase* biomeMap)
    {
        int width = DataManager::instance->requestValue("map_size"); 
        int height = width;
        double scale = DataManager::instance->requestValue("map_scale");

        ImageBase* imageColor = new ImageBase(width,height,true);


        unordered_map<int, vector<Point>> labels;
        labels = distanceSegmentation(image, width, height);

        labels = correctLabelError(labels, image);
        vector<Couleur> color;
        
        vector<InterestPoint*> interestPoints;

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

            InterestPoint* ip = new InterestPoint();
            ip->intId = image->get(val[0].x,val[0].y,0);
            ip->stringId = interestBaseTypes[ip->intId];
            ip->color = c;

            for (auto const& point : val) {

                imageColor->set(point.x, point.y, 0, c.r);
                imageColor->set(point.x, point.y, 1, c.g);
                imageColor->set(point.x, point.y, 2, c.b);
                ip->points.push_back(point);
            }

            interestPoints.push_back(ip);

            c.r = rand() % ((255 - 0) + 1) + 0;
            c.g = rand() % ((255 - 0) + 1) + 0;
            c.b = rand() % ((255 - 0) + 1) + 0;
        }

        generatePointsData(interestPoints,imageColor,image,biomeMap);

        return imageColor;
    }

    static void chooseIpType(InterestPoint* ip,ImageBase* biomeMap)
    {
        if(ip->stringId == "waterfall" || ip->stringId == "lake")
        {

        }
        else if(ip->stringId == "harbor")
        {
            int r =  (rand() / (float)RAND_MAX) * interestTypes["harbor"].size();
            ip->stringId = interestTypes["harbor"].at(r);
        }
        else if(ip->stringId == "meadow")
        {
            float p = (rand() / (float)RAND_MAX);
            if( p < forest_occurence &&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::desert_plain_color) &&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::desert_hills_color) &&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::desert_plateau_color)&&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::ice_plain_color) &&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::ice_hills_color) &&
                !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::ice_plateau_color))
            {
                ip->stringId = "forest"; return;
            }
             p = (rand() / (float)RAND_MAX);
            if( p < city_occurence &&
                 !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::desert_hills_color) &&
                  !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::jungle_hills_color) &&
                  !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::snow_hills_color) &&
                 !BiomeMap::isTheSameColor(ip->x,ip->y,biomeMap,BiomeMap::ice_hills_color ))
             {
                ip->stringId = "city"; return;
             }
        }
    }

    static void generatePointsData(vector<InterestPoint*> interestPoints,ImageBase* zones, ImageBase* points,ImageBase* biomeMap)
    {
        ofstream flux(ProjectManager::instance->projectPath() + "/" + PointsFileName);

        int i = 0;
        for(InterestPoint* ip : interestPoints)
        {
            int x = 0; int y = 0;
            for(Point p : ip->points)
            {
                x += p.x; y += p.y;
            }
            ip->x = (int)(x/(double)ip->points.size());
            ip->y = (int)(y/(double)ip->points.size());
            ip->size = ip->points.size();

            chooseIpType(ip,biomeMap);

            ip->name = generateName(ip);

            flux << i << " " << ip->stringId << " " << ip->color.r << ":" << ip->color.g << ":" << ip->color.b << " " <<
                ip->name << " " <<
                ip->x << ":" << ip->y << ":" << ip->size <<
                std::endl;
            i++;
        }
    }

    static string generateRandomName()
    {
        string str = "";

        for(int i = 0; i < nameParts.size();i++)
        {
            int n = (rand() / (float)RAND_MAX) * nameParts.at(i).size();
            str += nameParts.at(i).at(n);
        }

        return str;
    }

    static string generateName(InterestPoint* ip)
    {
        return ip->stringId + "_of_" + generateRandomName();
    }

    static void drawPoint(ImageBase* map, int x, int y, double radius)
    {
        int r = (int)radius;
        for(int dx = -r; dx <= r; dx++)
        {
            for(int dy = -r; dy <= r; dy++)
            {
                if(x + dx < 0 || x + dx >= map->getWidth() || y + dy < 0 || y + dy >= map->getHeight()){continue;}

                float d = sqrt((dx*dx) + (dy*dy));

                if(d <= radius)
                {
                    map->set(x + dx,y + dy,0,255);
                }
            }
        }
    }

    static vector<InterestPoint*> generatePointsList()
    {
        vector<InterestPoint*> list ;

        ifstream inFile;
        inFile.open(ProjectManager::instance->projectPath() + "/" + PointsFileName); //open the input file

        stringstream strStream;
        strStream << inFile.rdbuf(); //read the file

        string ipStr;


        while(std::getline(strStream, ipStr, '\n'))
        {
            stringstream ipSS; ipSS << ipStr;
            vector<stringstream*> values; string value; stringstream* svalue;
            while(std::getline(ipSS, value, ' '))
            {
                svalue = new stringstream(); *svalue << value;
                values.push_back(svalue);
            }

            string type = values.at(1)->str();
            stringstream* colors = values.at(2);
            string name = values.at(3)->str();
            stringstream* coords = values.at(4);

            string coord; int x,y,size; int i = 0;
            while(std::getline(*coords, coord, ':'))
            {
                if(i==0){x = stoi(coord);}
                if(i==1){y = stoi(coord);}
                if(i==2){size = stoi(coord);}
                i++;
            }

            string color; int r,b,g; i = 0;
            while(std::getline(*colors, color, ':'))
            {
                if(i==0){r = stoi(color);}
                if(i==1){g = stoi(color);}
                if(i==2){b = stoi(color);}
                i++;
            }

            replace(name.begin(), name.end(), '_', ' ');

            InterestPoint* ip = new InterestPoint();
            ip->x = x; ip->y = y; ip->size = size;
            ip->color = Couleur(r,g,b);
            ip->name = name; ip->stringId = type;

            list.push_back(ip);
        }

        return list;
    }

    static ImageBase* generatePoints(ImageBase* interestMap)
    {
        ImageBase* image = new ImageBase(interestMap->getWidth(),interestMap->getHeight(),false);

        vector<InterestPoint*> list = generatePointsList();
        double mapSize = DataManager::instance->requestValue("map_size");

        for(int i = 0; i < list.size();i++)
        {
            double r = (mapSize / 2048.0) * pointRadius * max(1.0,sqrt(list.at(i)->size)/10.0);
            drawPoint(image,list.at(i)->x, list.at(i)->y,r);
        }

        return image;
    }

};

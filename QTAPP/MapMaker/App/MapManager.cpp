#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <map>
#include <string>

#include "ProjectManager.h"
#include "MapManager.h"
#include "HeightMap.hpp"
#include "ReliefMap.hpp"
#include "RiverMap.hpp"
#include "PointOfInterestMap.hpp"
#include "ClimateMap.hpp"
#include "BiomeMap.hpp"

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"

using namespace std;
using namespace siv;

void MapManager::saveMap(string id)
{
    if(maps.count(id) > 0)
    {
        cout << ">> Saving <" << id << ">" << endl;
        maps[id]->save(ProjectManager::instance->projectPath() + "/MAPS/" + id + (maps[id]->getColor() ? ".ppm" : ".pgm"));
    }
}

void MapManager::loadMap(string file)
{
	string id = file.substr(0,file.size()-4);
	string path = ProjectManager::instance->projectPath() + "/MAPS/" + file;
	cout << "\t\t>> Loading map <" << id << "> from \"" << path << "\""<<endl;

    ImageBase* img = new ImageBase();
    img->load(path);

   	maps.insert_or_assign(id,img);
}

void MapManager::deleteMap(string name)
{
    if(maps.count(name) > 0)
    {
        maps.erase(name);
        remove((ProjectManager::instance->projectPath() + "/MAPS/" + name + ".pgm").c_str());
        remove((ProjectManager::instance->projectPath() + "/MAPS/" + name + ".ppm").c_str());
    }
}


ImageBase* MapManager::generateMap(string id)
{
    switch(hash_djb2a(id))
    {
        default: cout << ">> Requested map <" << id << "> doesnt exist ! " << endl; break;

        case "HEIGHT_LARGE"_sh: maps[id] = HeightMap::generateLargeImage(); break;
        case "HEIGHT_SMALL"_sh: maps[id] = HeightMap::generateSmallImage(); break;

        case "HEIGHT_BASE"_sh: maps[id] = HeightMap::baseMap(requestMap("HEIGHT_LARGE"), requestMap("HEIGHT_SMALL")); break;

        case "SEA_BINARY"_sh: maps[id] = HeightMap::seaBinaryMap(requestMap("HEIGHT_BASE"),maps.count(id) <= 0); break;

        case "HEIGHT_SEA"_sh: maps[id] = HeightMap::applySea(requestMap("HEIGHT_BASE")); break;
        case "HEIGHT_LAND"_sh: maps[id] = HeightMap::applyLand(requestMap("HEIGHT_SEA")); break;

        case "HEIGHT_GRADIENT"_sh: maps[id] = HeightMap::gradientMap(requestMap("HEIGHT_LAND")); break;
        case "HEIGHT_GRADIENT_R"_sh: maps[id] = HeightMap::getImageR(requestMap("HEIGHT_GRADIENT")); break;
        case "HEIGHT_GRADIENT_G"_sh: maps[id] = HeightMap::getImageG(requestMap("HEIGHT_GRADIENT")); break;
        case "HEIGHT_GRADIENT_B"_sh: maps[id] = HeightMap::getImageB(requestMap("HEIGHT_GRADIENT")); break;

        case "RIVER"_sh: maps[id] = RiverMap::riverMap(requestMap("HEIGHT_GRADIENT"),requestMap("HEIGHT_BASE"),requestMap("SEA_BINARY")); break;

        case "RELIEF"_sh: maps[id] = ReliefMap::reliefMap(requestMap("HEIGHT_GRADIENT"),requestMap("HEIGHT_BASE"),requestMap("SEA_BINARY"),requestMap("RIVER")); break;

        case "INTEREST"_sh: maps[id] = PointOfInterestMap::generatePointOfInterestMapGray(requestMap("HEIGHT_GRADIENT"),requestMap("RIVER"), requestMap("SEA_BINARY"), requestMap("RELIEF"), requestMap("HEIGHT_BASE")); break;
        case "INTEREST_C"_sh: maps[id] = PointOfInterestMap::generatePointOfInterestMapColored(requestMap("INTEREST")); break;
        case "INTEREST_POINTS"_sh: maps[id] = PointOfInterestMap::generatePoints(requestMap("INTEREST_C")); break;

        case "CLIMATE"_sh: maps[id] = ClimateMap::generateClimateMap(); break;

        case "BIOME"_sh: maps[id] = BiomeMap::generatePlainBiomeMap(requestMap("RELIEF"), requestMap("CLIMATE"),maps.count(id) <= 0); break;
    }
    return maps[id] ;
}

/*
void MapManager::getHeightMap(int width, int height) 
{
    maps["HEIGHT"] = HeighMap::generateHeightMap(width, height);
}

void MapManager::applyHeightMap(int width, int height) {
    maps["HEIGHT_SEUIL"] = HeighMap::seuilHeightMap(maps["HEIGHT"]); 
}*/

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

        case "HEIGHT_BASE"_sh: maps[id] = HeightMap::baseMap(); break;
        case "SEA_BINARY"_sh: maps[id] = HeightMap::seaBinaryMap(requestMap("HEIGHT_BASE")); break;

        case "HEIGHT_SEA"_sh: maps[id] = HeightMap::applySea(requestMap("HEIGHT_BASE")); break;
        case "HEIGHT_LAND"_sh: maps[id] = HeightMap::applyLand(requestMap("HEIGHT_SEA")); break;

        case "HEIGHT_GRADIENT"_sh: maps[id] = HeightMap::gradientMap(requestMap("HEIGHT_LAND")); break;

        case "RELIEF"_sh: maps[id] = ReliefMap::reliefMap(requestMap("HEIGHT_GRADIENT"),requestMap("HEIGHT_BASE")); break;

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

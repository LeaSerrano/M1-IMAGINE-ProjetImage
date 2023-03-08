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

ImageBase* MapManager::generateMap(string id)
{
    switch(hash_djb2a(id))
    {
        case "HEIGHT_BASE"_sh: maps[id] = HeighMap::generateHeightMap(); break;
        case "HEIGHT_SEA"_sh: maps[id] = HeighMap::seuilHeightMap(requestMap("HEIGHT_BASE")); break;
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
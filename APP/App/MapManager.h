#ifndef MAPMANAGER
#define MAPMANAGER

#include "../Library/ImageBase.h"
#include "../Library/PerlinNoise.hpp"

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <map>
#include <string>
#include <string_view>

using namespace std;
using namespace siv;


inline constexpr auto hash_djb2a(const std::string_view sv) {
    unsigned long hash{ 5381 };
    for (unsigned char c : sv) {
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

inline constexpr auto operator"" _sh(const char *str, size_t len) 
{
    return hash_djb2a(std::string_view{ str, len });
}


class MapManager
{
private:

    map<string,ImageBase*> maps;

public:

    static uint32_t seed() { return (uint32_t)((rand() / (float)RAND_MAX) * pow(2,32)); }

    ImageBase* get(string id)
    {
        if(maps.count(id) > 0)
        {
            return maps[id];
        }
        else
        {
            return nullptr;
        }
    }

    void saveMap(string id);
    void saveAllMaps()
    {
        for (map<string,ImageBase*>::const_iterator it=maps.begin(); it!=maps.end(); it++) 
        {
            saveMap(it->first);
        }
    }

    void loadMap(string name);

    void deleteMap(string name);

    ImageBase* requestMap(string id)
    {
        if(maps.count(id) > 0)
        {
            cout << ">> Using loaded map <" << id << ">" << endl;
            return maps[id];
        }
        else
        {
            cout << ">> Generating map <" << id << ">" << endl;
            return generateMap(id);
        }
    }

    ImageBase* generateMap(string id);

    void getHeightMap(int width, int height);

    void applyHeightMap(int width, int height);

    MapManager()
    {
        instance = this;
    }

    inline static MapManager* instance;
};

#endif

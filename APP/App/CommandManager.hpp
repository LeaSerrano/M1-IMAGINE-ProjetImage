#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <string>
#include <float.h>

using namespace std;

class CommandManager
{

public:

    void waitForEntry()
    {
        string line; getline(std::cin, line);
    }

   CommandManager()
   {
        instance = this;
   }

   inline static DataManager* instance;
};

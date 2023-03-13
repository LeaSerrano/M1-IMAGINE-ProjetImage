#pragma once

#include "../Library/ImageBase.h"

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

class Map
{
private:

    ImageBase* _image;
    bool _available;
    uint32_t _seed;

public:

    uint32_t seed() {return _seed;}


};

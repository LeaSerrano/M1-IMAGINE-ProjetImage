#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <filesystem>

#include <string>

#include "ProjectManager.h"
#include "MapManager.h"

void ProjectManager::loadMaps()
{
    std::string path = projectPath() + "/MAPS";
    cout << "\t>> Loading maps from \"" << path << "\"" << endl;
    
    for (const auto & entry : fs::directory_iterator(path))
    {
        MapManager::instance->loadMap(entry.path().filename());
    }
}

bool ProjectManager::loadProject()
{
    ifstream projectFile(projectFilePath().c_str());
    if(projectFile.good())
    {
        loadMaps();
        return true;
    }
    return false;
}

void ProjectManager::createProject()
{
    do
    {
        cout << "\t\t>> Enter project path : ";
        cin >> currentProjectPath;
        if(!fs::create_directory(currentProjectPath))
        {
            cout << "\t\t>> Error : bad path" << endl;
        }
        else
        {
            break;
        }
    }while(true);
   
    fs::create_directory(currentProjectPath + "/MAPS");

    ofstream pFile(currentProjectPath + "/project.dat");

    cout << "\t>> Project files created successfully" << endl;
}
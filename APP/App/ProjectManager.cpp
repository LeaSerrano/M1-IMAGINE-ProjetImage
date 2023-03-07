#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <filesystem>

#include <string>

#include "ProjectManager.h"
#include "MapManager.h"
#include "DataManager.hpp"

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
        loadData();
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

    ofstream pFile(currentProjectPath + "/project.txt");

    cout << "\t>> Project files created successfully" << endl;
}

void ProjectManager::saveData()
{
    ofstream pfile(currentProjectPath + "/project.txt");

    cout << "Saving data" << endl;

     for (map<string,double>::const_iterator it=DataManager::instance->values.begin(); it!=DataManager::instance->values.end(); it++) 
      {
          pfile << it->first;
          pfile << ':';
          pfile << it->second;
          pfile << '\n';
      }
}

void ProjectManager::loadData()
{
    ifstream pfile(currentProjectPath + "/project.txt");

    string id; string value;
    while ( pfile.good()) 
    {
        getline(pfile,id,':');
        if(!pfile.good()){break;}
        getline(pfile,value,'\n');
        
        DataManager::instance->setValue(id,stof(value));
    }

}

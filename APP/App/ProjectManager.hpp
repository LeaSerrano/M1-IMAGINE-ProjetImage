#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <string>

using namespace std;

class ProjectManager
{
private:

    string currentProjectPath;

public:

    const string projectPath() {return currentProjectPath;}
    const string projectFilePath() { return currentProjectPath + "/" + "project.dat";}

    bool loadProject()
    {
        ifstream projectFile(projectFilePath().c_str());
        return projectFile.good();
    }

    ProjectManager()
    {
        cout << "Instanciating <ProjectManager>" << endl;
        cout << "\t> Enter project path : ";
        cin >> currentProjectPath;

        if(loadProject())
        {
            cout << "\tProject loaded successfully"<<endl;
        }
        else
        {
            cout << "\tProject not found : creating a new one" <<endl;
        }

        instance = this;
    }

    inline static ProjectManager* instance;
};

#ifndef PROJECTMANAGER
#define PROJECTMANAGER

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>

using namespace std;
namespace fs = std::filesystem;

class ProjectManager
{
private:

    string currentProjectPath;

public:

    const string projectPath() {return currentProjectPath;}
    const string projectFilePath() { return currentProjectPath + "/" + "project.txt";}

    bool loadProject();
    void createProject();
    void createProject(string path);

    void loadMaps();

    void loadData();
    void saveData();

    void setup(string path)
    {            
        cout << ">> Instanciating <ProjectManager>" << endl;

        if(path == "")
        {       
            cout << "\t>> Enter project path : ";
            cin >> currentProjectPath;
        }
        else
        {
            cout << "\t>> Loading project at path : " << path << endl;
            currentProjectPath = path;
        }

        if(loadProject())
        {
            cout << "\t>> Project loaded successfully"<<endl;
        }
        else
        {
            cout << "\t>> Project not found : creating a new one" <<endl;
            createProject(path);
        }
    }

    ProjectManager(string path)
    {
        instance = this;

         setup(path);
    }

    ProjectManager()
    {
        instance = this;

        setup("");
    }

    inline static ProjectManager* instance;
};

#endif

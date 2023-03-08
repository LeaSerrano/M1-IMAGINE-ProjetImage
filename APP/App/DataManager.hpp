#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <string>

using namespace std;

class DataManager
{

public:

    map<string,double> values;

    double getValue(string id)
    {
      if(values.count(id) > 0)
      {
        return values[id];
      }
      return 0;
    }

    void setValue(string id, double v)
    {
      values[id] = v;
    }


    void display()
    {
      cout << ">> PROJECT DATA <<" << endl;

      for (map<string,double>::const_iterator it=values.begin(); it!=values.end(); it++) 
      {
          cout << it->first << "   " << it->second << endl;
      }
    }


  double requestValue(string name)
  {
    if(values.count(name) <= 0)
    {
      cout << ">> Enter <" << name << "> : ";
      double v;
      scanf("%lf",&v);
      setValue(name,v); 
    }
    else
    {
      cout << ">> Using <" << name << "> = " << getValue(name) << endl;

    }
    return getValue(name);
  }

   DataManager()
   {
        instance = this;
   }

   inline static DataManager* instance;
};
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <string>
#include <float.h>

using namespace std;

class DataManager
{

public:

   map<string,double> values;
   map<string,string> tooltips;

   bool valueExists(string name){return values.count(name) > 0;}

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

   //Checks if the value exists ; if it doesnt, starts a request
   void setupValue(string name)
   {
      if(valueExists(name))
      {
         cout << ">> Using <" << name << "> = " << getValue(name) << endl;
      }
      else
      {
         requestValue(name);
      }
   }

   double requestValue(string name)
   {
      return requestValue(name,DBL_MAX);
   }
   double requestValue(string name, double value)
   {
      return requestValue(name,value,false);
   }
   double requestValue(string name, bool ask)
   {
      return requestValue(name,DBL_MAX,ask);
   }
   double requestValue(string name, double value,bool ask)
   {
      if(value != DBL_MAX) { cout << ">> Suggested value for <" << name << "> : " << value << endl; }

      if(values.count(name) > 0)
      {
         if(ask)
         {
            cout << ">> Found <" << name << "> = " << getValue(name) << ". 'ENTER' to use it, or enter your own value : ";
            string line; getline(std::cin, line);
            if(line != "")
            {
               double v = stod(line);
               setValue(name,v);
            }
         }
         else
         {
            cout << ">> Using <" << name << "> = " << getValue(name) << endl;
         }
      }
      else
      {
         cout << ">> Enter <" << name << "> ";
         if(tooltips.count(name) > 0){cout << "(" << tooltips[name] << ") ";}
         cout << " : ";

         double v; scanf("%lf",&v);
         setValue(name,v);
      }

      return getValue(name);
   }

  void setupTooltips()
  {
      tooltips =
      {
          {"map_size","Size of the maps in pixels"},
          {"map_scale","Size of the maps in kilometers"}
      };
  }

   DataManager()
   {
        instance = this;
        setupTooltips();
   }

   inline static DataManager* instance;
};

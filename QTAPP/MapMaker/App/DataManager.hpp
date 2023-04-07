#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <map>
#include <string>
#include <float.h>
#include <thread>

using namespace std;

struct range
{
    double max; double min; double base;
    range(double _min, double _max, double _b){max = _max; min = _min; base = _b;}
    range(){}
};

class DataManager
{

public:

   bool isQT;

   map<string,double> values;

   map<string,string> tooltips;
   void setupTooltips()
   {
       tooltips =
           {
               {"map_size","Size of the maps in pixels"},
               {"map_scale","Size of the maps in kilometers"}
           };
   }

   map<string,range> ranges;
   void setupRanges()
   {
       ranges =
           {
               {"map_size",range(1024,8192,2048)},
               {"map_scale",range(25,500,200)},

               {"sea_level",range(0,1,0.5)},
               {"sea_slope",range(1,5,2)},
               {"shore_level",range(0.05,0.35,0.2)},
               {"beach_level",range(0.01,0.1,0.03)},
               {"beach_curve",range(1,5,2)},
               {"plain_level",range(0.2,0.5,0.45)},
               {"plain_curve",range(1,3,1.3)},
               {"hills_level",range(0.4,0.7,0.65)},
               {"hills_amplitude",range(0.01,0.1,0.03)},
               {"hills_frequency",range(1,10,5)},
               {"plateau_level",range(0.7,0.9,0.9)},
               {"plateau_curve",range(0.1,2,0.5)},
               {"peaks_curve",range(1,5,4)},

               {"desert_temperature",range(0,1,0.7)},
               {"ice_temperature",range(0,1,0.3)},
               {"high_humidity",range(0,1,0.7)},
               {"low_humidity",range(0,1,0.3)}
           };
   }

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
      if(value != DBL_MAX)
      {
         cout << ">> Suggested value for <" << name << "> : " << value << endl;
      }

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


   DataManager()
   {
        instance = this;
        setupTooltips();
        setupRanges();
   }

   inline static DataManager* instance;
};

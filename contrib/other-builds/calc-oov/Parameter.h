#pragma once

#include <string>

class Parameter
{
public:
  int method; // 1=simple, 2=compound
  bool outWords;
  bool lowercase;
  std::string juncture;
  
  Parameter();

};


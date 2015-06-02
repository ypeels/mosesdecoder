#pragma once

#include <string>

class Parameter
{
public:
  int method; // 1=lcs, 2=char-based
  
  std::string changeSourcePath;
  std::string prefix, suffix;
  
  Parameter();

};


#pragma once

#include <string>

class Parameter
{
public:
  int method; // 1=lcs, 2=char-based
  
  std::string newSplitPath;
  std::string prefix, suffix;
  
  Parameter();

};


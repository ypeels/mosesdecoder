#pragma once

#include <string>

class Parameter
{
public:
  int method; // 1=lcs, 2=char-based
  bool binarySplit;
  
  std::string juncturedPath;
  std::string prefix, suffix;
  
  Parameter();

};


#include <iostream>
#include "Base.h"

using namespace std;

void Base::OutputOOV(const std::unordered_set<std::string> &oovTypes)
{
  for (auto c : oovTypes)
  {
    std::cout << c << " ";
  }
  
}

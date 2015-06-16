#include <iostream>
#include "Base.h"

using namespace std;

void Base::OutputOOV(const std::set<std::string> &oovTypes)
{
  for (auto c : oovTypes)
  {
    std::cout << c << " ";
  }
  
}

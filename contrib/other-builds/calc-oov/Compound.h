#pragma once
#include <string>
#include <unordered_set>
#include "Base.h"

class Compound: public Base
{
public:
  void CreateVocab(std::ifstream &corpusStrme);
  void CalcOOV(std::ifstream &testStrme);

protected:
  	std::unordered_set<std::string> vocab;


};
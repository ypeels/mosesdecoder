#pragma once
#include <string>
#include <unordered_set>
#include "Base.h"

class Simple: public Base
{
public:
  Simple();
  void CreateVocab(std::ifstream &corpusStrme, const Parameter &params);
  void CalcOOV(std::unordered_set<std::string> &oovTypes,
              std::ifstream &testStrme, const Parameter &params) const;

protected:
  	std::unordered_set<std::string> vocab;


};
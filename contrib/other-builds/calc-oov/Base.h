#pragma once
#include <fstream>
#include <set>
#include <string>

class Parameter;

class Base
{
public:
  virtual ~Base() {}
  virtual void CreateVocab(std::ifstream &corpusStrme, const Parameter &params) = 0;
  virtual void CalcOOV(std::set<std::string> &oovTypes,
                      std::ifstream &testStrme, const Parameter &params) const = 0;
  static void OutputOOV(const std::set<std::string> &oovTypes);

};


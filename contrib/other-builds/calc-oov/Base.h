#pragma once
#include <fstream>

class Parameter;

class Base
{
public:
  virtual ~Base() {}
  virtual void CreateVocab(std::ifstream &corpusStrme, const Parameter &params) = 0;
  virtual void CalcOOV(std::ifstream &testStrme, const Parameter &params) const = 0;
  

};


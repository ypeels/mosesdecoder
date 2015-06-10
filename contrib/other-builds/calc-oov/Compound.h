#pragma once
#include <string>
#include <unordered_set>
#include "Base.h"
#include "Node.h"

class Compound: public Base
{
public:
  void CreateVocab(std::ifstream &corpusStrme);
  void CalcOOV(std::ifstream &testStrme) const;

protected:
  Node m_root;
  
  bool Decode(const std::string &tok) const;
  bool Decode(std::unordered_set<size_t> &stack, const std::string &tok, size_t startPos) const;
};
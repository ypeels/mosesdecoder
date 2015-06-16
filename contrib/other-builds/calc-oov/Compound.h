#pragma once
#include <string>
#include "Base.h"
#include "Node.h"

class Compound: public Base
{
public:
  Compound();
  void CreateVocab(std::ifstream &corpusStrme, const Parameter &params);
  void CalcOOV(std::unordered_set<std::string> &oovTypes,
              std::ifstream &testStrme, const Parameter &params) const;

protected:
  Node m_root;
  const Node *m_rootJuncture;
  
  bool Decode(const std::string &tok, const Parameter &params) const;
  bool Decode(std::unordered_set<size_t> &stack, 
              const std::string &tok, size_t startPos,
              const Parameter &params) const;
};
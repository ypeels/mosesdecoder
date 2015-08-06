#pragma once
#include <vector>
#include <map>

namespace Moses
{
class HypothesisStack;
class Hypothesis;

class LatticeRescorer
{
public:
  
  void Rescore(const std::vector < HypothesisStack* > &stacks);
  
protected:
  typedef std::vector<const Hypothesis*> HypoList;
  typedef std::map<const Hypothesis*, HypoList> FwdPtrs;
  FwdPtrs m_fwdPtrs;
  
  void Rescore(HypothesisStack &stack);
  
};
  
}


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
  
  void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);

protected:
  typedef std::vector<Hypothesis*> HypoList;
  typedef std::map<Hypothesis*, HypoList> FwdPtrs;
  std::vector<FwdPtrs> m_fwdPtrsColl;
  
  void Rescore(HypothesisStack &stack, Hypothesis *hypo, size_t pass);

};
  
}


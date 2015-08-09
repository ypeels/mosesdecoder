#pragma once
#include <vector>
#include <map>
#include "TypeDef.h"

namespace Moses
{
class HypothesisStack;
class Hypothesis;

class LatticeRescorer
{
public:
  
  void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);

protected:
  std::vector < HypothesisStack* > *m_stacks;

  typedef std::vector<Hypothesis*> HypoList;
  typedef std::map<Hypothesis*, HypoList> FwdPtrs;
  std::vector<FwdPtrs> m_fwdPtrsColl;
  
  void Rescore(HypothesisStack &stack, HypoList &fwdHypos, Hypothesis *hypo, size_t pass);
  std::pair<AddStatus, const Hypothesis*>
  	  Rescore1Hypo(HypothesisStack &stack, Hypothesis *hypo, size_t pass);

  void AttachLosers(const Hypothesis *hypo);

  void DeleteHypo(Hypothesis *hypo);
  void Multiply(const Hypothesis *hypo, const std::set<const Hypothesis*> &nodes);
  void Multiply(const Hypothesis *hypo, const Hypothesis *prevHypo);

  void OutputStackSize() const;
};
  
}


#include <boost/foreach.hpp>
#include <iostream>
#include <set>

#include "LatticeRescorer.h"
#include "Hypothesis.h"
#include "HypothesisStack.h"
#include "moses/FF/StatelessFeatureFunction.h"
#include "moses/FF/StatefulFeatureFunction.h"

using namespace std;

namespace Moses
{
void LatticeRescorer::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
  // create forward pointers
  m_fwdPtrsColl.resize(stacks.size());

  // all stacks, except 1st
  for (int stackInd = stacks.size() - 1; stackInd > 0; --stackInd) {
    HypothesisStack &stack = *stacks[stackInd];
    
    // 1 stack
    HypothesisStack::const_iterator iterStack;
    for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
      Hypothesis *hypo = *iterStack;
      Hypothesis *prevHypo = const_cast<Hypothesis*>(hypo->GetPrevHypo());
      size_t numWordsCovered = prevHypo->GetWordsBitmap().GetNumWordsCovered();

      FwdPtrs &fwdPtrs = m_fwdPtrsColl[numWordsCovered];
      HypoList &list = fwdPtrs[prevHypo];
      list.push_back(hypo);
    } //for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {

    stack.DetachAll();

  } //for (iterStacks = stacks.rbegin(); iterStacks != stacks.rend(); ++iterStacks) {
  
  // rescore
  for (size_t stackInd = 1; stackInd < stacks.size(); ++stackInd) {
      HypothesisStack &stack = *stacks[stackInd];
      FwdPtrs &fwdPtrs = m_fwdPtrsColl[stackInd];

      BOOST_FOREACH(FwdPtrs::value_type val, fwdPtrs ) {
        Hypothesis *hypo = val.first;
        HypoList &fwdHypos = val.second;
        Rescore(stack, fwdHypos, hypo, pass);
      }
  }
}

void LatticeRescorer::Rescore(HypothesisStack &stack, HypoList &fwdHypos, Hypothesis *hypo, size_t pass)
{
  ArcList arcs;
  if (hypo->GetArcList()) {
    arcs = *hypo->GetArcList();
    hypo->ClearArcList();
  }

  set<const Hypothesis*> nodes;

  std::pair<AddStatus, const Hypothesis*> status;
  status = Rescore1Hypo(stack, hypo, pass);

  switch (status.first) {
  case New:
    nodes.insert(hypo);
    break;
  case Pruned:
    break;
  default:
    UTIL_THROW2("Impossible");
  }

  // losers list
  BOOST_FOREACH(Hypothesis *arc, arcs) {
    status = Rescore1Hypo(stack, arc, pass);

    switch (status.first) {
    case New:
      nodes.insert(hypo);
      break;
    case Pruned:
      //DeleteHypo(hypo);
      break;
    case RecombinedWin: {
      const Hypothesis *loser = status.second;
      size_t ret = nodes.erase(loser);
      assert(ret == 1);
      nodes.insert(hypo);
      break;
    }
    case RecombinedLose:
      break;
    }
  }

  // sort out graph.
  if (nodes.size() == 0) {
    // the node has been deleted. Delete all fwd hypos, won't be reachable anymore
    DeleteHypo(hypo);
  }
  else {
    const Hypothesis *prevHypo = *nodes.begin();
    if (prevHypo != hypo) {
      // winning hypo has changed
      BOOST_FOREACH(Hypothesis *nextHypo, fwdHypos) {
        nextHypo->SetPrevHypo(hypo);
      }
    }
    nodes.erase(nodes.begin());

    // add the rest
    Multiply(hypo, nodes);
  }
}

std::pair<AddStatus, const Hypothesis*> LatticeRescorer::Rescore1Hypo(HypothesisStack &stack, Hypothesis *hypo, size_t pass)
{
  const std::vector<FeatureFunction*> &ffs = FeatureFunction::GetFeatureFunctions(pass);
  size_t sfInd = 0;
  BOOST_FOREACH(FeatureFunction *ff, ffs) {
      if (ff->IsStateless()) {
        StatelessFeatureFunction *slFF = static_cast<StatelessFeatureFunction*>(ff);
        hypo->EvaluateWhenApplied(*slFF);
      }
      else {
        StatefulFeatureFunction *sfFF = static_cast<StatefulFeatureFunction*>(ff);
        hypo->EvaluateWhenApplied(*sfFF, sfInd);
        ++sfInd;
      }
  }

  std::pair<AddStatus, const Hypothesis*> status = stack.AddPrune(hypo);
  return status;
}

void LatticeRescorer::Multiply(const Hypothesis *hypo, const std::set<const Hypothesis*> &nodes)
{
  //FwdPtrs &fwdPtrs = m_fwdPtrsColl[numWordsCovered];
  //HypoList &list = fwdPtrs[hypo];

  BOOST_FOREACH(const Hypothesis *fwdOrigHypo, nodes) {
    Multiply(fwdOrigHypo, hypo);
  }
}

void LatticeRescorer::Multiply(const Hypothesis *hypo, const Hypothesis *prevHypo)
{
    Hypothesis *newFwdHypo = new Hypothesis(*hypo, *prevHypo);

    size_t numWordsCovered = newFwdHypo->GetWordsBitmap().GetNumWordsCovered();
    FwdPtrs &fwdPtrs = m_fwdPtrsColl[numWordsCovered];
    HypoList &list = fwdPtrs[newFwdHypo];
    assert(list.size() == 0);

}

void LatticeRescorer::DeleteHypo(Hypothesis *hypo)
{
  delete hypo;

  // delete all hypos that depend on curr hypo
  size_t numWordsCovered = hypo->GetWordsBitmap().GetNumWordsCovered();
  FwdPtrs &fwdPtrs = m_fwdPtrsColl[numWordsCovered];
  FwdPtrs::const_iterator iter = fwdPtrs.find(hypo);
  assert(iter != fwdPtrs.end());

  const HypoList &list = iter->second;
  BOOST_FOREACH(Hypothesis *nextHypo, list) {
      DeleteHypo(nextHypo);
  }

}

} // namespace


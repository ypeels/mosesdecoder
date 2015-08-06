#include <boost/foreach.hpp>
#include <iostream>

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
      
      // losers list
      const ArcList *arcs = hypo->GetArcList();
      if (arcs) {
        BOOST_FOREACH(Hypothesis *arc, *arcs) {
            list.push_back(arc);
        } 
      }
    } //for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {

    stack.DetachAll();

  } //for (iterStacks = stacks.rbegin(); iterStacks != stacks.rend(); ++iterStacks) {
  
  // rescore
  for (size_t stackInd = 1; stackInd < stacks.size(); ++stackInd) {
      HypothesisStack &stack = *stacks[stackInd];
      FwdPtrs &fwdPtrs = m_fwdPtrsColl[stackInd];

      BOOST_FOREACH(FwdPtrs::value_type val, fwdPtrs ) {
        Hypothesis *hypo =val.first;
        Rescore(stack, hypo, pass);
      }
  }
}

void LatticeRescorer::Rescore(HypothesisStack &stack, Hypothesis *hypo, size_t pass)
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

    stack.AddPrune(hypo);

}

} // namespace


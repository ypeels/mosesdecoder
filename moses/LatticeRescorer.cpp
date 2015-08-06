#include <boost/foreach.hpp>
#include <iostream>

#include "LatticeRescorer.h"
#include "Hypothesis.h"
#include "HypothesisStack.h"

using namespace std;

namespace Moses
{
void LatticeRescorer::Rescore(const std::vector < HypothesisStack* > &stacks)
{
  // create forward pointers
  // all stacks
  std::vector < HypothesisStack* >::const_reverse_iterator iterStacks;
  for (iterStacks = stacks.rbegin(); iterStacks != stacks.rend(); ++iterStacks) {
    HypothesisStack &stack = **iterStacks;
    
    // 1 stack
    HypothesisStack::const_iterator iterStack;
    for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
      Hypothesis *hypo = *iterStack;
      const Hypothesis *prevHypo = hypo->GetPrevHypo();
      HypoList &list = m_fwdPtrs[prevHypo];
      list.push_back(hypo);
      
      /*
      // losers list
      const ArcList *arcs = hypo->GetArcList();
      if (arcs) {
        BOOST_FOREACH(const Hypothesis *arc, *arcs )
        {
            list.push_back(arc);
        } 
      }
      */
    } //for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
  } //for (iterStacks = stacks.rbegin(); iterStacks != stacks.rend(); ++iterStacks) {
  
  // rescore
  BOOST_FOREACH(HypothesisStack *stack, stacks ) {
      cerr << stack;
      Rescore(*stack);
  }
}

void LatticeRescorer::Rescore(HypothesisStack &stack)
{
  vector<Hypothesis*> hypoList;
  HypothesisStack::const_iterator iterStack;
  for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
    Hypothesis *hypo = *iterStack;
    hypoList.push_back(hypo);
  }
  
  stack.DetachAll();
    
  // rescore each hypo and add back to stack
  BOOST_FOREACH(Hypothesis *hypo, hypoList )
  {
      
  } 
}

}


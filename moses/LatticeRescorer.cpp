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
LatticeRescorerNode::LatticeRescorerNode(Hypothesis *bestHypo)
:m_bestHypo(bestHypo)
{
	m_hypos.insert(m_bestHypo);
}

void LatticeRescorerNode::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
	boost::unordered_set<const Hypothesis*> newWinners;

    // rescore each hypo
    BOOST_FOREACH(Hypothesis *hypo, m_hypos) {
	    size_t numWordsCovered = hypo->GetWordsBitmap().GetNumWordsCovered();
	    HypothesisStack &stack = *stacks[numWordsCovered];

	    std::pair<AddStatus, const Hypothesis*> status;
	    status = Rescore1Hypo(stack, hypo, pass);

	    switch (status.first) {
	    case New:
	    	newWinners.insert(hypo);
	      break;
	    case Pruned:
	      cerr << "pruned " << hypo << endl;
	      break;
	    case RecombinedWin: {
	      const Hypothesis *loser = status.second;
	      size_t ret = newWinners.erase(loser);
	      assert(ret == 1);
	      newWinners.insert(hypo);
	      break;
	    }
	    case RecombinedLose:
	      break;
	    }
    }

    // Done rescoring. Sort out graph.
    if (newWinners.size() == 0) {
      // the node has been deleted. Delete all fwd hypos, won't be reachable anymore
      //cerr << "nothing here " << hypo << endl;
      DeleteFwdHypos();
    }
    else {
      const Hypothesis *prevHypo = *newWinners.begin();
      if (prevHypo != m_bestHypo) {
        // winning hypo has changed
        BOOST_FOREACH(const Edge &edge, m_fwdNodes) {
        	Hypothesis *nextHypo = edge.second;
        	nextHypo->SetPrevHypo(prevHypo);
        }
      }
      newWinners.erase(newWinners.begin());

      // add the rest
      Multiply(newWinners);
    }
}

std::pair<AddStatus, const Hypothesis*> LatticeRescorerNode::
	Rescore1Hypo(HypothesisStack &stack, Hypothesis *hypo, size_t pass)
{
  const std::vector<FeatureFunction*> &ffs = FeatureFunction::GetFeatureFunctions(pass);
  BOOST_FOREACH(FeatureFunction *ff, ffs) {
      if (ff->IsStateless()) {
        StatelessFeatureFunction *slFF = static_cast<StatelessFeatureFunction*>(ff);
        hypo->EvaluateWhenApplied(*slFF);
      }
      else {
        StatefulFeatureFunction *sfFF = static_cast<StatefulFeatureFunction*>(ff);
        hypo->EvaluateWhenApplied(*sfFF, sfFF->GetStatefulId());
      }
  }

  std::pair<AddStatus, const Hypothesis*> status = stack.AddPrune(hypo);
  return status;
}

void LatticeRescorerNode::DeleteFwdHypos()
{
    BOOST_FOREACH(const Edge &edge, m_fwdNodes) {
    	LatticeRescorerNode *nextNode = edge.first;
    	Hypothesis *nextHypo = edge.second;

    	nextNode->m_hypos.erase(nextHypo);
    	delete nextHypo;
    }
}

void LatticeRescorerNode::Multiply(const boost::unordered_set<const Hypothesis*> &newWinners)
{

}

std::ostream& operator<<(std::ostream &out, const LatticeRescorerNode &obj)
{
	out << "[" << &obj << "," << obj.m_hypos.size() << "," << obj.m_fwdNodes.size() << "] " << flush;
	return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

LatticeRescorerNode &LatticeRescorerGraph::AddFirst(Hypothesis *bestHypo)
{
	LatticeRescorerNode &firstNode = Add(bestHypo);
	m_firstNode = &firstNode;
	return firstNode;
}

LatticeRescorerNode &LatticeRescorerGraph::Add(Hypothesis *bestHypo)
{
  std::pair<Coll::iterator, bool> retPair = m_nodes.insert(LatticeRescorerNode(bestHypo));
  LatticeRescorerNode &ret = const_cast<LatticeRescorerNode&>(*retPair.first);

  Hypothesis *prevHypo = const_cast<Hypothesis *>(bestHypo->GetPrevHypo());
  if (prevHypo) {
	  LatticeRescorerNode &prevNode = Find(prevHypo);
	  LatticeRescorerNode::Edge edge(&ret, bestHypo);
	  prevNode.Add(edge);
  }

  const ArcList *arcs = bestHypo->GetArcList();
  if (arcs) {
	  // losers list
	  BOOST_FOREACH(Hypothesis *arc, *arcs) {
		  Hypothesis *prevHypo = const_cast<Hypothesis *>(arc->GetPrevHypo());
		  LatticeRescorerNode &prevNode = Find(prevHypo);
		  LatticeRescorerNode::Edge edge(&ret, arc);
		  prevNode.Add(edge);
	  }
  }

  return ret;
}

LatticeRescorerNode &LatticeRescorerGraph::Find(Hypothesis *bestHypo)
{
  Coll::iterator iter = m_nodes.find(LatticeRescorerNode(bestHypo));
  assert(iter != m_nodes.end());

  LatticeRescorerNode &retNonConst = const_cast<LatticeRescorerNode&>(*iter);

  return retNonConst;
}

void LatticeRescorerGraph::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
	cerr << "rescoring pass " << pass << endl;

	boost::unordered_set<LatticeRescorerNode::Edge> &fwdEdges = m_firstNode->m_fwdNodes;
	BOOST_FOREACH(const LatticeRescorerNode::Edge &edge, fwdEdges) {
		LatticeRescorerNode &nextNode = *edge.first;
		nextNode.Rescore(stacks, pass);
	}
}

std::ostream& operator<<(std::ostream &out, const LatticeRescorerGraph &obj)
{
	out << obj.m_nodes.size() << " nodes: ";
	BOOST_FOREACH(const LatticeRescorerNode &node, obj.m_nodes) {
		out << node << " ";
	}
	return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeRescorer::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
  // empty hypo
  Hypothesis *firstHypo = *stacks[0]->begin();
  m_graph.AddFirst(firstHypo);

  // add all hypos
  for (size_t stackInd = 1; stackInd < stacks.size(); ++stackInd) {
    cerr << "adding stack " << stackInd << endl;
    HypothesisStack &stack = *stacks[stackInd];

    // 1 stack
    HypothesisStack::const_iterator iterStack;
    for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
      Hypothesis *hypo = *iterStack;
      m_graph.Add(hypo);
    }

    stack.DetachAll();
  }

  cerr << m_graph << endl;

  // rescore
  m_graph.Rescore(stacks, pass);
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


void LatticeRescorer::OutputStackSize() const
{
  cerr << "stack size:";
  BOOST_FOREACH(const HypothesisStack *stack, *m_stacks) {
      cerr << stack->size() << " ";
  }

  cerr << "fwd ptr size:";
  BOOST_FOREACH(const FwdPtrs &fwdPtrs, m_fwdPtrsColl) {
      cerr << fwdPtrs.size() << " ";
  }

  cerr << endl;
}

} // namespace


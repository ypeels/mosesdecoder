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
LatticeRescorerNode::LatticeRescorerNode(const Hypothesis *bestHypo, const Hypothesis *prevHypo)
:m_bestHypo(bestHypo)
,m_prevHypo(prevHypo)
{
}

void LatticeRescorerNode::Add(Hypothesis *hypo)
{
	/*
	cerr << "  node=" << this
		<< "  adding " << hypo << " " << hypo->GetWordsBitmap()
		<< " best=" << m_bestHypo << " " << m_bestHypo->GetWordsBitmap() << endl;
	*/
	m_hypos.insert(hypo);
}

void LatticeRescorerNode::Add(const Edge &edge)
{
	/*
	cerr << "  linking " << this
		<< " to " << edge.first
		<< "  hypo=" << edge.second << " " << edge.second->GetWordsBitmap() << endl;
	*/
	m_fwdNodes.insert(edge);
}

void LatticeRescorerNode::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
	cerr << "BEFORE:";
	OutputStackSize(stacks);

	boost::unordered_set<const Hypothesis*> newWinners;

    // rescore each hypo
    BOOST_FOREACH(Hypothesis *hypo, m_hypos) {

    	cerr << "node=" << this << " "
    		 <<	"rescoring " << hypo << " " << hypo->GetWordsBitmap()
    		 << " prev=" << hypo->GetPrevHypo() << " " << hypo->GetPrevHypo()->GetWordsBitmap() << endl;


	    size_t numWordsCovered = hypo->GetWordsBitmap().GetNumWordsCovered();
	    HypothesisStack &stack = *stacks[numWordsCovered];

	    std::pair<AddStatus, const Hypothesis*> status;
	    status = Rescore1Hypo(stack, hypo, pass);

	    switch (status.first) {
	    case New:
	    	newWinners.insert(hypo);
	      break;
	    case Pruned:
	      //cerr << "pruned " << hypo << endl;
	      break;
	    case RecombinedWin: {
	      const Hypothesis *loser = status.second;
	      size_t ret = newWinners.erase(loser);
	      //assert(ret == 1); // loser may be from another node
	      newWinners.insert(hypo);
	      break;
	    }
	    case RecombinedLose:
	      const Hypothesis *winner = status.second;
	      newWinners.insert(hypo); // winner may be from another node
	      break;
	    }

    }

	cerr << "AFTER:";
	OutputStackSize(stacks);

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

    // next nodes
	BOOST_FOREACH(const LatticeRescorerNode::Edge &edge, m_fwdNodes) {
		LatticeRescorerNode &nextNode = *edge.first;
		nextNode.Rescore(stacks, pass);
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

void LatticeRescorerNode::OutputStackSize(const std::vector < HypothesisStack* > &stacks) const
{
  cerr << "stack size:";
  BOOST_FOREACH(const HypothesisStack *stack, stacks) {
      cerr << stack->size() << " ";
  }

  cerr << endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeRescorerGraph::AddFirst(Hypothesis *bestHypo)
{
	LatticeRescorerNode &node = AddNodeNode(bestHypo, bestHypo->GetPrevHypo());
	node.Add(bestHypo);

	m_firstNode = &node;
}

void LatticeRescorerGraph::Add(Hypothesis *bestHypo)
{
  //cerr << "best     " << bestHypo << " " << bestHypo->GetWordsBitmap() << endl;
  LatticeRescorerNode &node = AddNodeNode(bestHypo, bestHypo->GetPrevHypo());
  node.Add(bestHypo);

  Hypothesis *prevHypo = const_cast<Hypothesis *>(bestHypo->GetPrevHypo());
  if (prevHypo) {
	  LatticeRescorerNode &prevNode = Find(prevHypo);
	  LatticeRescorerNode::Edge edge(&node, bestHypo);
	  prevNode.Add(edge);
  }

  const ArcList *arcs = bestHypo->GetArcList();
  if (arcs) {
	  // losers list
	  BOOST_FOREACH(Hypothesis *arc, *arcs) {
		  LatticeRescorerNode &node = AddNodeNode(bestHypo, arc->GetPrevHypo());
		  node.Add(arc);

		  Hypothesis *prevHypo = const_cast<Hypothesis *>(arc->GetPrevHypo());
		  LatticeRescorerNode &prevNode = Find(prevHypo);
		  LatticeRescorerNode::Edge edge(&node, arc);
		  prevNode.Add(edge);
	  }
  }
}

LatticeRescorerNode &LatticeRescorerGraph::AddNodeNode(const Hypothesis *bestHypo, const Hypothesis *prevHypo)
{
  LatticeRescorerNode *findNode = new LatticeRescorerNode(bestHypo, prevHypo);
  std::pair<Coll::iterator, bool> retPair = m_nodes.insert(findNode);

  if (!retPair.second) {
	  // already there
	  delete findNode;
  }

  LatticeRescorerNode &node = **retPair.first;
  return node;
}

LatticeRescorerNode &LatticeRescorerGraph::Find(Hypothesis *bestHypo)
{
  //cerr << "finding " << bestHypo << endl;
  LatticeRescorerNode *findNode = new LatticeRescorerNode(bestHypo, bestHypo->GetPrevHypo());
  Coll::iterator iter = m_nodes.find(findNode);
  assert(iter != m_nodes.end());
  delete findNode;

  LatticeRescorerNode &ret = **iter;
  return ret;
}

void LatticeRescorerGraph::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
	cerr << "rescoring pass " << pass << endl;
	cerr << "first node " << m_firstNode << " " << m_firstNode->m_bestHypo << " " << m_firstNode->m_bestHypo->GetWordsBitmap() << endl;

	boost::unordered_set<LatticeRescorerNode::Edge> &fwdEdges = m_firstNode->m_fwdNodes;
	BOOST_FOREACH(const LatticeRescorerNode::Edge &edge, fwdEdges) {
		LatticeRescorerNode &nextNode = *edge.first;
		nextNode.Rescore(stacks, pass);
	}
}

std::ostream& operator<<(std::ostream &out, const LatticeRescorerGraph &obj)
{
	out << obj.m_nodes.size() << " nodes: ";
	BOOST_FOREACH(LatticeRescorerNode *node, obj.m_nodes) {
		out << *node << " ";
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
  OutputStackSize(stacks);
}

void LatticeRescorer::OutputStackSize(const std::vector < HypothesisStack* > &stacks) const
{
  cerr << "stack size:";
  BOOST_FOREACH(const HypothesisStack *stack, stacks) {
      cerr << stack->size() << " ";
  }

  cerr << endl;
}

} // namespace


#include <boost/foreach.hpp>
#include <iostream>
#include <set>
#include <algorithm>

#include "LatticeRescorer.h"
#include "Hypothesis.h"
#include "HypothesisStack.h"
#include "HypothesisStackNormal.h"
#include "moses/FF/StatelessFeatureFunction.h"
#include "moses/FF/StatefulFeatureFunction.h"

using namespace std;

namespace Moses
{
extern bool g_mosesDebug;

LatticeRescorerNode::LatticeRescorerNode(const Hypothesis *bestHypo)
:m_bestHypo(bestHypo)
{
}

Hypos &LatticeRescorerNode::Add(Hypothesis *hypo)
{
	const Hypothesis *prevHypo = hypo->GetPrevHypo();
	HyposPerPrevHypo::iterator iter = m_hypos.find(prevHypo);

	Hypos *hypos;
	if (iter == m_hypos.end()) {
		hypos = &m_hypos[prevHypo];
		hypos->m_container = this;
		hypos->m_prevHypo = prevHypo;
	}
	else {
		hypos = &iter->second;
	}
/*
	cerr << "  adding " << hypo << " " << hypo->GetWordsBitmap()
		<< " best=" << m_bestHypo << " " << m_bestHypo->GetWordsBitmap()
		<< " to " << this << " " << hypos
		<< endl;
*/
	hypos->m_hypos.insert(hypo);

	return *hypos;
}

void LatticeRescorerNode::AddEdge(Hypos &edge)
{
//	cerr << "adding edge " << &edge << " to " << this
//			<< endl;

	m_fwdNodes.insert(&edge);
}

void LatticeRescorerNode::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass, Hypos *hypos)
{
	/*
	cerr << "rescoring all hypos in " << hypos->m_container << " " << hypos << " "
			<< m_bestHypo->GetWordsBitmap()
			<< endl;
	cerr << "best " << m_bestHypo << " " << m_bestHypo->GetWordsBitmap() << endl;
	cerr << "BEFORE:";
	OutputStackSize(stacks);
	*/

	// rescore each hypo
    BOOST_FOREACH(Hypothesis *hypo, hypos->m_hypos) {
    	/*
    	cerr <<	"rescoring " << hypo
    		<< " " << hypo->GetWordsBitmap()
    		<< " prev=" << hypo->GetPrevHypo() //<< " " << hypo->GetPrevHypo()->GetWordsBitmap()
			 << endl;
		*/
	    size_t numWordsCovered = hypo->GetWordsBitmap().GetNumWordsCovered();
	    HypothesisStack &stack = *stacks[numWordsCovered];

	    std::pair<AddStatus, const Hypothesis*> status;
	    status = Rescore1Hypo(stack, hypo, pass);

	    switch (status.first) {
	    case New:
	    	m_newWinners.insert(hypo);
	      break;
	    case Pruned:
	      //cerr << "pruned " << hypo << endl;
	      assert(false); // can't deal with pruning @ the mo
	      break;
	    case RecombinedWin: {
	      const Hypothesis *loser = status.second;

	      /*
	      cerr << "winners & losers:" << loser << " ";
	      std::ostream_iterator<const Hypothesis*> out_it (std::cerr,", ");
	      std::copy( m_newWinners.begin(), m_newWinners.end(), out_it );
	      cerr << endl;
			*/

	      size_t ret = m_newWinners.erase(loser);
	      assert(ret == 1);
	      m_newWinners.insert(hypo);
	      break;
	    }
	    case RecombinedLose:
	      break;
	    }

    }

	//cerr << "AFTER:";
	//OutputStackSize(stacks);

	m_hypos.erase(hypos->m_prevHypo);
	if (!m_hypos.empty()) {
		// not all hypos in this node are done yet
		return;
	}

    // Done rescoring. Sort out graph.
    if (m_newWinners.size() == 0) {
      // the node has been deleted. Delete all fwd hypos, won't be reachable anymore
      //cerr << "nothing here " << hypo << endl;
      assert(false); // can't deal with pruned hypos @ the mo
      //DeleteFwdHypos();
    }
    else {
      const Hypothesis *prevHypo = *m_newWinners.begin();
      if (prevHypo != m_bestHypo) {
        // winning hypo has changed
        BOOST_FOREACH(const Hypos *hypos, m_fwdNodes) {
            BOOST_FOREACH(Hypothesis *nextHypo, hypos->m_hypos) {
				nextHypo->SetPrevHypo(prevHypo);
            }
        }
      }
      m_newWinners.erase(m_newWinners.begin());

      // add the rest
      Multiply();
    }

    // next nodes
	BOOST_FOREACH(Hypos *hypos, m_fwdNodes) {
		LatticeRescorerNode *node = hypos->m_container;
		node->Rescore(stacks, pass, hypos);
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

  hypo->CalcTotalScore();

  std::pair<AddStatus, const Hypothesis*> status = stack.AddNoPrune(hypo);
  return status;
}

void LatticeRescorerNode::DeleteFwdHypos()
{
	//cerr << "delete " << this << endl;
    BOOST_FOREACH(Hypos *hypos, m_fwdNodes) {
    	hypos->m_container->DeleteHypos(hypos);
    }
}

void LatticeRescorerNode::DeleteHypos(Hypos *hypos)
{
    BOOST_FOREACH(Hypothesis *hypo, hypos->m_hypos) {
    	delete hypo;
    }
    m_hypos.erase(hypos->m_prevHypo);
    delete hypos;
}

void LatticeRescorerNode::Multiply()
{
	//cerr << "m_newWinners=" << m_newWinners.size() << endl;
	BOOST_FOREACH(const Hypothesis *winner, m_newWinners) {
		BOOST_FOREACH(Hypos *hypos, m_fwdNodes) {
			Multiply(*hypos, winner);
		}
	}
}

void LatticeRescorerNode::Multiply(Hypos &hypos, const Hypothesis *prevHypo)
{
	boost::unordered_set<Hypothesis*> newHypos;

	BOOST_FOREACH(const Hypothesis *origNextHypo, hypos.m_hypos) {
		Hypothesis *newHypo = new Hypothesis(*origNextHypo, *prevHypo);
		newHypos.insert(newHypo);
	}

	// add all new hypo into existing hypos coll
	std::copy(newHypos.begin(), newHypos.end(), std::inserter(hypos.m_hypos, hypos.m_hypos.end()));
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
	LatticeRescorerNode &node = AddNode(bestHypo);
	node.Add(bestHypo);

	m_firstNode = &node;
}

void LatticeRescorerGraph::Add(Hypothesis *bestHypo)
{
  //cerr << "best     " << bestHypo << " " << bestHypo->GetWordsBitmap() << endl;
  LatticeRescorerNode &node = AddNode(bestHypo);
  Hypos &currHypos = node.Add(bestHypo);

  Hypothesis *prevHypo = const_cast<Hypothesis *>(bestHypo->GetPrevHypo());
  if (prevHypo) {
	  LatticeRescorerNode &prevNode = AddNode(prevHypo);
	  prevNode.AddEdge(currHypos);
  }

  const ArcList *arcs = bestHypo->GetArcList();
  if (arcs) {
	  // losers list
	  BOOST_FOREACH(Hypothesis *arc, *arcs) {
		  Hypothesis *prevHypo = const_cast<Hypothesis *>(arc->GetPrevHypo());

		  Hypos &arcHypos = node.Add(arc);

		  LatticeRescorerNode &prevNode = AddNode(prevHypo);
		  prevNode.AddEdge(arcHypos);
	  }
	  bestHypo->ClearArcList();
  }
}

LatticeRescorerNode &LatticeRescorerGraph::AddNode(const Hypothesis *bestHypo)
{
  LatticeRescorerNode *node;

  Coll::iterator iter = m_nodes.find(bestHypo);
  if (iter == m_nodes.end()) {
	  // not found
	  node = new LatticeRescorerNode(bestHypo);
	  m_nodes[bestHypo] = node;
  }
  else {
	  node = iter->second;
  }

  return *node;
}

void LatticeRescorerGraph::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
	/*
	cerr << "first node " << m_firstNode << " " << m_firstNode->m_bestHypo << " " << m_firstNode->m_bestHypo->GetWordsBitmap()
		<< " " << m_firstNode->m_hypos.size()
		<< endl;
	*/

	LatticeRescorerNode::FwdNodes &fwdNodes = m_firstNode->m_fwdNodes;
	BOOST_FOREACH(Hypos *hypos, fwdNodes) {
		LatticeRescorerNode *node = hypos->m_container;
		node->Rescore(stacks, pass, hypos);
	}
}

std::ostream& operator<<(std::ostream &out, const LatticeRescorerGraph &obj)
{
	out << obj.m_nodes.size() << " nodes: ";
	BOOST_FOREACH(const LatticeRescorerGraph::Coll::value_type &objPair, obj.m_nodes) {
		LatticeRescorerNode *node = objPair.second;
		out << *node << " ";
	}
	return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeRescorer::Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass)
{
  g_mosesDebug = true;
  cerr << "rescoring pass " << pass << endl;

  // create graph
  Hypothesis *firstHypo = *stacks[0]->begin();
  m_graph.AddFirst(firstHypo);

  // add all hypos
  for (size_t stackInd = 1; stackInd < stacks.size(); ++stackInd) {
    //cerr << "add stack " << stackInd << endl;
    HypothesisStack &stack = *stacks[stackInd];

    // 1 stack
    HypothesisStack::const_iterator iterStack;
    for (iterStack = stack.begin(); iterStack != stack.end(); ++iterStack) {
      Hypothesis *hypo = *iterStack;
      m_graph.Add(hypo);
    }

    stack.DetachAll();
  }

  //cerr << m_graph << endl;

  // change graph before rescoring
  const std::vector<FeatureFunction*> &ffs = FeatureFunction::GetFeatureFunctions(pass);
  BOOST_FOREACH(FeatureFunction *ff, ffs) {
	  ff->ChangeLattice(m_graph);
  }

  // rescore
  m_graph.Rescore(stacks, pass);
  OutputStackSize(stacks);

  // fix up arc lists
  for (size_t stackInd = 0; stackInd < stacks.size(); ++stackInd) {
    HypothesisStack *stack = stacks[stackInd];
    HypothesisStackNormal *stackNormal
    = static_cast<HypothesisStackNormal*>(stack);

    stackNormal->CleanupArcList();
  }
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


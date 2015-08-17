#pragma once
#include <vector>
#include <map>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include "TypeDef.h"

namespace Moses
{
class HypothesisStack;
class Hypothesis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LatticeRescorerNode
{
  friend std::ostream& operator<<(std::ostream&, const LatticeRescorerNode&);

  void OutputStackSize(const std::vector < HypothesisStack* > &stacks) const;

public:
    struct Hypos {
    	boost::unordered_set<Hypothesis*> m_hypos;
    	LatticeRescorerNode *m_container;
    	const Hypothesis *m_prevHypo; // key in container

    };

	const Hypothesis *m_bestHypo;

	typedef boost::unordered_map<const Hypothesis*, Hypos> HyposPerPrevHypo;
	HyposPerPrevHypo m_hypos;

	typedef boost::unordered_set<Hypos*> FwdNodes;
	FwdNodes m_fwdNodes;

	boost::unordered_set<const Hypothesis*> m_newWinners;

	LatticeRescorerNode(const Hypothesis *bestHypo);

	inline bool operator==(const LatticeRescorerNode &other) const
	{ return m_bestHypo == other.m_bestHypo; }

	Hypos &Add(Hypothesis *hypo);
	void AddEdge(Hypos &edge);

	void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass, Hypos *hypos);
	std::pair<AddStatus, const Hypothesis*> Rescore1Hypo
		(HypothesisStack &stack, Hypothesis *hypo, size_t pass);
	void DeleteFwdHypos();
	void DeleteHypos(Hypos *hypos);

	void Multiply();
	void Multiply(Hypos &hypos, const Hypothesis *prevHypo);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LatticeRescorer
{
	  friend std::ostream& operator<<(std::ostream&, const LatticeRescorer&);
public:
  
  void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);

protected:
  std::vector < HypothesisStack* > *m_stacks;

  typedef boost::unordered_map<const Hypothesis*, LatticeRescorerNode*> Coll;
  //typedef std::set<LatticeRescorerNode*, LatticeRescorerNodeComparer> Coll;
  Coll m_nodes;
  LatticeRescorerNode *m_firstNode;

  void OutputStackSize(const std::vector < HypothesisStack* > &stacks) const;

	void AddFirst(Hypothesis *bestHypo);
	void Add(Hypothesis *bestHypo);

	LatticeRescorerNode &AddNode(const Hypothesis *bestHypo);

};
  
}


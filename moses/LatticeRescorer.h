#pragma once
#include <vector>
#include <map>
#include <boost/unordered_set.hpp>
#include "TypeDef.h"

namespace Moses
{
class HypothesisStack;
class Hypothesis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LatticeRescorerNode
{
  friend std::ostream& operator<<(std::ostream&, const LatticeRescorerNode&);

public:
	typedef std::pair<LatticeRescorerNode*, Hypothesis*> Edge;

	Hypothesis *m_bestHypo;
	boost::unordered_set<Hypothesis*> m_hypos;
	boost::unordered_set<Edge> m_fwdNodes;

	LatticeRescorerNode(Hypothesis *bestHypo);

	inline bool operator==(const LatticeRescorerNode &other) const
	{ return m_bestHypo == other.m_bestHypo; }

	void Add(Hypothesis *hypo)
	{ m_hypos.insert(hypo); }

	void Add(const Edge &edge)
	{ m_fwdNodes.insert(edge); }

	void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);
	std::pair<AddStatus, const Hypothesis*> Rescore1Hypo
		(HypothesisStack &stack, Hypothesis *hypo, size_t pass);
	void DeleteFwdHypos();
	void Multiply(const boost::unordered_set<const Hypothesis*> &newWinners);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline size_t hash_value(const LatticeRescorerNode &node)
{ return (size_t) node.m_bestHypo; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LatticeRescorerGraph
{
  friend std::ostream& operator<<(std::ostream&, const LatticeRescorerGraph&);

public:
	typedef boost::unordered_set<LatticeRescorerNode> Coll;
	Coll m_nodes;
	LatticeRescorerNode *m_firstNode;

	LatticeRescorerNode &AddFirst(Hypothesis *bestHypo);
	LatticeRescorerNode &Add(Hypothesis *bestHypo);
	LatticeRescorerNode &Find(Hypothesis *bestHypo);

	void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LatticeRescorer
{
public:
  
  void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass);

protected:
  std::vector < HypothesisStack* > *m_stacks;
  LatticeRescorerGraph m_graph;

  typedef std::vector<Hypothesis*> HypoList;
  typedef std::map<Hypothesis*, HypoList> FwdPtrs;
  std::vector<FwdPtrs> m_fwdPtrsColl;
  
  void Multiply(const Hypothesis *hypo, const std::set<const Hypothesis*> &nodes);
  void Multiply(const Hypothesis *hypo, const Hypothesis *prevHypo);

  void OutputStackSize() const;


};
  
}


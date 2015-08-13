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

    };

	const Hypothesis *m_bestHypo;

	typedef boost::unordered_map<const Hypothesis*, Hypos> HyposPerPrevHypo;
	HyposPerPrevHypo m_hypos;

	typedef boost::unordered_set<Hypos*> FwdNodes;
	FwdNodes m_fwdNodes;

	LatticeRescorerNode(const Hypothesis *bestHypo);

	inline bool operator==(const LatticeRescorerNode &other) const
	{ return m_bestHypo == other.m_bestHypo; }

	Hypos &Add(Hypothesis *hypo);
	void AddEdge(Hypos &edge);

	void Rescore(const std::vector < HypothesisStack* > &stacks, size_t pass, Hypos &hypos);
	std::pair<AddStatus, const Hypothesis*> Rescore1Hypo
		(HypothesisStack &stack, Hypothesis *hypo, size_t pass);
	void DeleteFwdHypos();
	void Multiply(const boost::unordered_set<const Hypothesis*> &newWinners);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LatticeRescorerNodeComparer
{
public:
    std::size_t operator()(const LatticeRescorerNode *node) const
    {
		//std::cerr << "hashing " << node << std::endl;

		size_t seed = (size_t) node->m_bestHypo;
	   // boost::hash_combine(seed, node->m_prevHypo);

		return seed;
    }

    bool operator()( const LatticeRescorerNode *lhs, const LatticeRescorerNode *rhs ) const
	{
		//std::cerr << "comparing== " << lhs << " " << rhs << std::endl;
		return lhs->m_bestHypo == rhs->m_bestHypo;
	}

    /*
    bool operator()( const LatticeRescorerNode *lhs, const LatticeRescorerNode *rhs ) const
	{
		//std::cerr << "comparing< " << lhs << " " << rhs << std::endl;
		return lhs->m_bestHypo < rhs->m_bestHypo;
	}*/

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LatticeRescorerGraph
{
  friend std::ostream& operator<<(std::ostream&, const LatticeRescorerGraph&);

public:
	typedef boost::unordered_set<LatticeRescorerNode*, LatticeRescorerNodeComparer, LatticeRescorerNodeComparer> Coll;
	//typedef std::set<LatticeRescorerNode*, LatticeRescorerNodeComparer> Coll;
	Coll m_nodes;
	LatticeRescorerNode *m_firstNode;

	void AddFirst(Hypothesis *bestHypo);
	void Add(Hypothesis *bestHypo);

	LatticeRescorerNode &AddNodeNode(const Hypothesis *bestHypo);

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

  void OutputStackSize(const std::vector < HypothesisStack* > &stacks) const;


};
  
}


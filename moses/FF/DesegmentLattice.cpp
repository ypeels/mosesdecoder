#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "DesegmentLattice.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/LatticeRescorer.h"
#include "moses/Hypothesis.h"
#include "moses/StaticData.h"
#include "moses/TargetPhrase.h"
#include "moses/WordsBitmap.h"
#include "moses/TranslationOption.h"

using namespace std;

namespace Moses
{
DesegmentLattice::DesegmentLattice(const std::string &line)
  :StatelessFeatureFunction(0, line)
{
  m_tuneable = false;
  m_doJoining = true;
  ReadParameters();
}

void DesegmentLattice::EvaluateInIsolation(const Phrase &source
                                       , const TargetPhrase &targetPhrase
                                       , ScoreComponentCollection &scoreBreakdown
                                       , ScoreComponentCollection &estimatedFutureScore) const
{}

void DesegmentLattice::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void DesegmentLattice::EvaluateTranslationOptionListWithSourceContext(const InputType &input

    , const TranslationOptionList &translationOptionList) const
{}

void DesegmentLattice::EvaluateWhenApplied(const Hypothesis& hypo,
                                       ScoreComponentCollection* accumulator) const
{}

void DesegmentLattice::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                       ScoreComponentCollection* accumulator) const
{}

void DesegmentLattice::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "arg") {
    // set value here
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

/////////////////////////////////////////////////////////////////////////////////
//Lattice desegmentation

void DesegmentLattice::ChangeLattice(LatticeRescorerGraph &graph) const
{
  cerr << "Before:" << endl << graph << endl;

  LatticeRescorerNode::FwdNodes &fwdHypos = graph.m_firstNode->m_fwdNodes;
  BOOST_FOREACH(Hypos *hypos, fwdHypos) {
    ChangeLattice(hypos);
  }

  // replace and delete old segmented hypos. Insert new desegmented hypos
  const HypoReplaceColl &coll = GetHypoReplaceCache();

  boost::unordered_map<Hypos*, Hypos*> replaceFwdNodes;

  BOOST_FOREACH(const HypoReplace &hypoReplace, coll) {
	  assert(hypoReplace.out.size() == hypoReplace.nodes.size());

	  const Hypothesis *firstHypo = hypoReplace.out.front();
	  const Hypothesis *prevHypo= firstHypo->GetPrevHypo();

	  // delete old hypos
	  Hypos *hypos;
	  Hypothesis *hypo;
	  for (size_t i = 0; i < hypoReplace.out.size(); ++i) {
		  hypos = hypoReplace.nodes[i];
		  hypo = hypoReplace.out[i];

		  hypos->m_hypos.erase(hypo);
	  }

	  // is last hypo the winning hypo? Reset prev-hypo for next hypos
	  LatticeRescorerNode *node = hypos->m_container;
	  if (node->m_bestHypo == hypo) {
		  ResetPrevHypo(*node, node->m_bestHypo, hypo);
	  }

	  // add new hypo
	  Hypothesis *newHypo = hypoReplace.in;
	  newHypo->SetPrevHypo(prevHypo);
	  cerr << "newHypo=" << newHypo->GetCurrTargetPhrase() << endl;

	  Hypos *lastHypos = hypoReplace.nodes.back();
	  node = lastHypos->m_container;
	  Hypos &newHypos = node->Add(newHypo);

	  // reset fwd nodes of prev hypo
	  Hypos *firstHypos = hypoReplace.nodes.front();
	  replaceFwdNodes[firstHypos] = &newHypos;
  }

  // actually do reset fwd nodes of prev hypo
  ResetFwdNodes(graph, replaceFwdNodes);

  cerr << "After:" << endl << graph << endl;
}

void DesegmentLattice::ResetFwdNodes(LatticeRescorerGraph &graph
								, const boost::unordered_map<Hypos*, Hypos*> &replaceFwdNodes) const
{
  // recursively do next nodes 1st
  BOOST_FOREACH(LatticeRescorerGraph::Coll::value_type &objPair, graph.m_nodes) {
	  LatticeRescorerNode &node = *objPair.second;
	  LatticeRescorerNode::FwdNodes &fwdNodes = node.m_fwdNodes;

	  boost::unordered_map<Hypos*, Hypos*>::const_iterator iter;
	  for (iter = replaceFwdNodes.begin(); iter != replaceFwdNodes.end(); ++iter) {
		  Hypos *oldHypos = iter->first;
		  Hypos *newHypos = iter->second;

		  LatticeRescorerNode::FwdNodes::const_iterator iterFwdNodes = fwdNodes.find(oldHypos);
		  if (iterFwdNodes != fwdNodes.end()) {
			  fwdNodes.erase(iterFwdNodes);
			  fwdNodes.insert(newHypos);
		  }
	  }

  }


}


void DesegmentLattice::ResetPrevHypo(LatticeRescorerNode &node, const Hypothesis *oldPrevHypo, const Hypothesis *newPrevHypo) const
{
	BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {
		BOOST_FOREACH(Hypothesis *nextHypo, nextHypos->m_hypos) {
			nextHypo->SetPrevHypo(newPrevHypo);
		}

		// reconnect to right place in node
		nextHypos->m_prevHypo = newPrevHypo;

		LatticeRescorerNode *nextNode = nextHypos->m_container;
		nextNode->m_hypos.erase(oldPrevHypo);
		nextNode->m_hypos[newPrevHypo] = nextHypos;
	}

}


void DesegmentLattice::ChangeLattice(Hypos *hypos) const
{
  LatticeRescorerNode &node = *hypos->m_container;
  const Hypothesis *bestHypo = node.m_bestHypo;
  const Phrase &tp = bestHypo->GetCurrTargetPhrase();

  string tpStr;
  size_t juncture = Desegment(tpStr, tp);
  /*
  cerr << "A tp=" << tp
       << " tpStr=" << tpStr
       << " juncture=" << juncture << endl;
  */
  if ((juncture & 2) == 0) {
    // don't extend last word
    BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {
      ChangeLattice(nextHypos);
    }
  }
  else {
    // last word is part of a compound which extends to the next hypo (potentially)
	HypoReplace hypoReplace;
	hypoReplace.out.push_back(const_cast<Hypothesis*>(bestHypo));
	hypoReplace.nodes.push_back(hypos);

    MergeHypos(tpStr, hypoReplace, node);
  }
}

void DesegmentLattice::MergeHypos(const std::string &tpStrOrig, const HypoReplace &hyposReplacedOrig, LatticeRescorerNode &node) const
{
  BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {
	  bool done = false;
	  BOOST_FOREACH(Hypothesis *nextHypo, nextHypos->m_hypos) {
		string tpStr = tpStrOrig;

		HypoReplace hyposReplaced(hyposReplacedOrig);
		hyposReplaced.out.push_back(nextHypo);
		hyposReplaced.nodes.push_back(nextHypos);

		const Phrase &tp = nextHypo->GetCurrTargetPhrase();
		size_t juncture = Desegment(tpStr, tp);

		/*
		cerr << "B tp=" << tp
			 << " tpStr=" << tpStr
			 << " juncture=" << juncture << endl;
		*/

		if (juncture & 2) {
		  // don't extend last word
   		  done = true;

		  LatticeRescorerNode &nextNode = *nextHypos->m_container;
		  MergeHypos(tpStr, hyposReplaced, nextNode);
		}
		else {
			// whole compound word formed. Time to create hypo.
			CreateHypo(tpStr, hyposReplaced);
		}
	  }

	  if (!done) {
		  ChangeLattice(nextHypos);
	  }
  }

}

void DesegmentLattice::CreateHypo(const std::string str, HypoReplace hyposReplaced) const
{
  const StaticData &sd = StaticData::Instance();
  const std::vector<FactorType> &outFactors = sd.GetOutputFactorOrder();

  const Hypothesis *firstHypo = hyposReplaced.out.front();
  const Hypothesis *prevHypo = firstHypo->GetPrevHypo();
  const Hypothesis *lastHypo = hyposReplaced.out.back();

  TargetPhrase tp;
  tp.CreateFromString(Output, outFactors, str, NULL);

  // using range of 1st hypo - false
  const WordsRange &range = firstHypo->GetCurrSourceWordsRange();
  TranslationOption *transOpt = new TranslationOption(range, tp);

  CacheColl &transOptCache = GetCache();
  transOptCache.push_back(transOpt);

  Hypothesis *newHypo = new Hypothesis(*lastHypo, *prevHypo);
  newHypo->SetTranslationOption(*transOpt);

  hyposReplaced.in = newHypo;
  GetHypoReplaceCache().push_back(hyposReplaced);

  //cerr << "tp=" << tp << endl;
  //cerr << "newHypo=" << *newHypo << endl;

}

size_t DesegmentLattice::Desegment(std::string &tpStr, const Phrase &in) const
{
//	const StaticData &sd = StaticData::Instance();

  size_t juncture;
  for (size_t pos = 0; pos < in.GetSize(); ++pos) {
    const Word &inWord = in.GetWord(pos);
    string str;
    juncture = HasJuncture(inWord, str);
    if (juncture == 0) {
      tpStr += " " + str + " ";
    } else if (juncture == 1) {
      tpStr += str + " ";
    } else if (juncture == 2) {
      tpStr += " " + str;
    } else {
      assert(juncture == 3);
      tpStr += str;
    }
  }

  return juncture;
}

size_t DesegmentLattice::HasJuncture(const Word &word, std::string &stripped) const
{
  size_t juncture = 0;
  stripped = Trim(word.ToString());
  if (stripped[0] == '+') {
    juncture += 1;
  }
  if (stripped[stripped.size()-1] == '+') {
    juncture += 2;
  }

  if (juncture & 1) {
    stripped = stripped.substr(1, stripped.size() - 1);
  }
  if (juncture & 2) {
    stripped = stripped.substr(0, stripped.size() - 1);
  }

  return juncture;
}

DesegmentLattice::CacheColl &DesegmentLattice::GetCache() const
{
  CacheColl *cache;
  cache = m_cache.get();
  if (cache == NULL) {
    cache = new CacheColl;
    m_cache.reset(cache);
  }
  assert(cache);
  return *cache;
}

DesegmentLattice::HypoReplaceColl &DesegmentLattice::GetHypoReplaceCache() const
{
  HypoReplaceColl *cache;
  cache = m_hypoReplace.get();
  if (cache == NULL) {
    cache = new HypoReplaceColl;
    m_hypoReplace.reset(cache);
  }
  assert(cache);
  return *cache;
}


void DesegmentLattice::CleanUpAfterSentenceProcessing(const InputType& source)
{
	CacheColl &coll = GetCache();
	RemoveAllInColl(coll);
}

} // namespace


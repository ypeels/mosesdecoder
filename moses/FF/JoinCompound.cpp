#include <vector>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "JoinCompound.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/LatticeRescorer.h"
#include "moses/Hypothesis.h"
#include "moses/StaticData.h"
#include "moses/TargetPhrase.h"

using namespace std;

namespace Moses
{
JoinCompound::JoinCompound(const std::string &line)
  :StatelessFeatureFunction(0, line)
{
  m_tuneable = false;
  m_doJoining = true;
  ReadParameters();
}

void JoinCompound::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void JoinCompound::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void JoinCompound::EvaluateTranslationOptionListWithSourceContext(const InputType &input

    , const TranslationOptionList &translationOptionList) const
{}

void JoinCompound::EvaluateWhenApplied(const Hypothesis& hypo,
    ScoreComponentCollection* accumulator) const
{}

void JoinCompound::EvaluateWhenApplied(const ChartHypothesis &hypo,
    ScoreComponentCollection* accumulator) const
{}

void JoinCompound::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "arg") {
    // set value here
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

void JoinCompound::DoJoin(std::string &output)
{  
  boost::replace_all(output, "+ +", "");
  boost::replace_all(output, "+ ", " ");
  boost::replace_all(output, " +", " ");
  
  // begining of sentence
  if (output.at(0) == '+' && output.size() > 1 && output.at(1) != ' ') {
    output.erase(0, 1);
  }
}

void JoinCompound::ChangeLattice(LatticeRescorerGraph &graph) const
{
	cerr << "Start ChangeLattice" << graph << endl;

	LatticeRescorerNode::FwdNodes &fwdHypos = graph.m_firstNode->m_fwdNodes;
	BOOST_FOREACH(Hypos *hypos, fwdHypos) {
		ChangeLattice(hypos);
	}
}

void JoinCompound::ChangeLattice(Hypos *hypos) const
{
	LatticeRescorerNode &node = *hypos->m_container;
	const Hypothesis *bestHypo = node.m_bestHypo;
	const Phrase &tp = bestHypo->GetCurrTargetPhrase();

	string tpStr;
	size_t juncture = Desegment(tpStr, tp);
	cerr << "A tp=" << tp
		<< " tpStr=" << tpStr
		<< " juncture=" << juncture << endl;

	if ((juncture & 2) == 0) {
		// don't extend last word
		BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {
			ChangeLattice(nextHypos);
		}
	}
	else {
		// last word is part of a compound which extends to the next hypo (potentially)
		BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {
			MergeHypos(tpStr, bestHypo, nextHypos);
		}
	}
}

void JoinCompound::MergeHypos(const std::string &tpStrOrig, const Hypothesis *currHypo, Hypos *hypos) const
{
	size_t juncture = 0;
	BOOST_FOREACH(Hypothesis *nextHypo, hypos->m_hypos) {
		string tpStr = tpStrOrig;
		const Phrase &tp = nextHypo->GetCurrTargetPhrase();
		juncture = Desegment(tpStr, tp);

		cerr << "B tp=" << tp
			<< " tpStr=" << tpStr
			<< " juncture=" << juncture << endl;
	}

	if ((juncture & 2) == 0) {
		// don't extend last word
		boost::unordered_set<LatticeRescorerNode*> nodes;
		LatticeRescorerNode &node = *hypos->m_container;
		BOOST_FOREACH(Hypos *nextHypos, node.m_fwdNodes) {

		}

		//ChangeLattice(nextHypos);
}
	else {

	}

}

size_t JoinCompound::Desegment(std::string &tpStr, const Phrase &in) const
{
//	const StaticData &sd = StaticData::Instance();

	size_t juncture;
	for (size_t pos = 0; pos < in.GetSize(); ++pos) {
		const Word &inWord = in.GetWord(pos);
		string str;
		juncture = HasJuncture(inWord, str);
		if (juncture == 0) {
			tpStr += " " + str + " ";
		}
		else if (juncture == 1) {
			tpStr += str + " ";
		}
		else if (juncture == 2) {
			tpStr += " " + str;
		}
		else {
			assert(juncture == 3);
			tpStr += str;
		}
	}

	return juncture;
}

size_t JoinCompound::HasJuncture(const Word &word, std::string &stripped) const
{
	size_t juncture = 0;
	stripped = Trim(word.ToString());
	if (stripped.front() == '+') {
		juncture += 1;
	}
	if (stripped.back() == '+') {
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

} // namespace


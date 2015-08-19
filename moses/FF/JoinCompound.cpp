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

	stringstream strme;
	size_t juncture = Desegment(strme, tp);
	cerr << "tp=" << tp
		<< " juncture=" << juncture << endl;

	if ((juncture & 2) == 0) {
		// don't extend last word
		BOOST_FOREACH(Hypos *hypos, node.m_fwdNodes) {
			ChangeLattice(hypos);
		}
	}
	else {
		// last word is part of a compound which extends to the next hypo (potentially)
		BOOST_FOREACH(Hypos *hypos, node.m_fwdNodes) {
			BOOST_FOREACH(Hypothesis *nextHypo, hypos->m_hypos) {
				MergeHypos(strme, bestHypo, nextHypo);

				// create new tp
				TargetPhrase newTP;

			}
		}
	}
}

void JoinCompound::MergeHypos(std::stringstream &strme, const Hypothesis *currHypo, Hypothesis *nextHypo) const
{
	const Phrase &tp = nextHypo->GetCurrTargetPhrase();
	Desegment(strme, tp);
}

size_t JoinCompound::Desegment(stringstream &strme, const Phrase &in) const
{
	const StaticData &sd = StaticData::Instance();

	size_t juncture;
	for (size_t pos = 0; pos < in.GetSize(); ++pos) {
		const Word &inWord = in.GetWord(pos);
		juncture = HasJuncture(inWord);
		if (juncture == 0) {
			//out.AddWord(inWord);
		}
		else {
			// a morpheme
			string str = inWord.ToString();
			if (juncture & 1) {
				str = str.substr(1, str.size() - 1);
			}
			if (juncture & 2) {
				str = str.substr(0, str.size() - 1);
			}

			if ((juncture & 2) == 0) {
				// end of compound word. Write out word
				strme << str;
				cerr << "strme=" << strme.str() << endl;
				const std::vector<FactorType> &fo = sd.GetOutputFactorOrder();

				Word newWord(false);
				newWord.CreateFromString(Output, fo, strme.str(), false);
				//out.AddWord(newWord);
				strme.clear();
			}
			else {
				strme << str;
			}

		}
	}

	return juncture;
}

size_t JoinCompound::HasJuncture(const Word &word) const
{
	size_t ret = 0;
	string str = Trim(word.ToString());
	if (str.front() == '+') {
		ret += 1;
	}
	if (str.back() == '+') {
		ret += 2;
	}
	return ret;
}

} // namespace


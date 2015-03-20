#include <vector>
#include "RuleDiscr.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/ChartTranslationOptionList.h"
#include "moses/ChartTranslationOptions.h"
#include "moses/PP/CountsPhraseProperty.h"

using namespace std;

namespace Moses
{
RuleDiscr::RuleDiscr(const std::string &line)
  :StatelessFeatureFunction(1, line)
{
  ReadParameters();
}

std::vector<float> RuleDiscr::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 1,
                 "SyntaxRHS must only have 1 score");
  vector<float> ret(1, 1);
  return ret;
}

void RuleDiscr::EvaluateInIsolation(const Phrase &source
                                      , const TargetPhrase &targetPhrase
                                      , ScoreComponentCollection &scoreBreakdown
                                      , ScoreComponentCollection &estimatedFutureScore) const
{
  targetPhrase.SetRuleSource(source);
}

void RuleDiscr::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{
}

void RuleDiscr::EvaluateWhenApplied(const Hypothesis& hypo,
                                      ScoreComponentCollection* accumulator) const
{}

void RuleDiscr::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                      ScoreComponentCollection* accumulator) const
{}

void RuleDiscr::EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList) const
{
  FeatureFunction &pt = FeatureFunction::FindFeatureFunction("TranslationModel0");
	
  // find max p(e|f)
  float maxPEF = - std::numeric_limits<float>::infinity();
  //cerr << "ChartTranslationOptionList:" << endl;
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    //cerr << "ChartTranslationOptions " << i << "=" << transOpts.GetSize() << endl;

    for (size_t j = 0; j < transOpts.GetSize(); ++j) {
    	const ChartTranslationOption &transOpt = transOpts.Get(j);
    	cerr << "   " << transOpt << endl;

		std::vector<float> scores = transOpt.GetScores().GetScoresForProducer(&pt);
		float pef = scores[2];
		
		if (maxPEF < pef) {
			maxPEF = pef;
		}
    }
  }

  // set scores
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    //cerr << "ChartTranslationOptions " << i << "=" << transOpts.GetSize() << endl;

    for (size_t j = 0; j < transOpts.GetSize(); ++j) {
    	ChartTranslationOption &transOpt = transOpts.Get(j);
    	cerr << "   " << transOpt << endl;

		std::vector<float> scores = transOpt.GetScores().GetScoresForProducer(&pt);
		float diff = maxPEF - scores[2];

		transOpt.GetScores().PlusEquals(this, diff);
	}
  }
}

void RuleDiscr::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "min-count") {
    //m_minCount = Scan<float>(value);
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

}


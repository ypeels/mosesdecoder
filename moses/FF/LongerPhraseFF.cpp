#include <vector>
#include "LongerPhraseFF.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/StaticData.h"
#include "moses/TranslationOptionCollection.h"
#include "moses/TranslationModel/PhraseDictionary.h"

using namespace std;

namespace Moses
{
LongerPhraseFF::LongerPhraseFF(const std::string &line)
  :StatelessFeatureFunction(1, line)
{
  ReadParameters();
}

void LongerPhraseFF::EvaluateInIsolation(const Phrase &source
                                   , const TargetPhrase &targetPhrase
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection &estimatedFutureScore) const
{
}

void LongerPhraseFF::EvaluateWithSourceContext(const InputType &input
                                   , const InputPath &inputPath
                                   , const TargetPhrase &targetPhrase
                                   , const StackVec *stackVec
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection *estimatedFutureScore) const
{
}

void LongerPhraseFF::EvaluateTranslationOptionListWithSourceContext(const InputType &input

                , const TranslationOptionList &translationOptionList) const
{}

void LongerPhraseFF::EvaluateGivenAllOtherTransOpts(const InputType &input
		  	  	  	    , const WordsRange &range
		  	  	  	  	, const TranslationOptionCollection &transOptColl
                        , const TranslationOptionList &translationOptionList) const
{
	int inputSize = input.GetSize();
	int startPos = range.GetStartPos();
	int width = range.GetNumWordsCovered();

	float maxTransProb = GetMax(inputSize, startPos, width, transOptColl);

}

float LongerPhraseFF::GetMax(int inputSize, int startPos, int width
		,const TranslationOptionCollection &transOptColl) const
{
	float ret = - std::numeric_limits<float>::infinity();

	for (int currWidth = width + 1; currWidth <= inputSize; ++currWidth) {
		for (int currStartPos = 0; currStartPos <= startPos; ++currStartPos) {
			int currEndPos = currStartPos + currWidth - 1;

			if (currEndPos >= inputSize) {
				break;
			}

			const TranslationOptionList &currTransOpts = transOptColl.GetTranslationOptionList(WordsRange(currStartPos, currEndPos));
			float currMax = GetMax(currTransOpts);
			if (currMax > ret) {
				ret = currMax;
			}
		}

	}

	return ret;
}

float LongerPhraseFF::GetMax(const TranslationOptionList &currTransOpts) const
{
	float ret = - std::numeric_limits<float>::infinity();

	PhraseDictionary *pt = PhraseDictionary::GetColl()[0];

	TranslationOptionList::const_iterator iter;
	for (iter = currTransOpts.begin(); iter != currTransOpts.end(); ++iter) {
		const TranslationOption &transOpt = **iter;
		const ScoreComponentCollection &scores = transOpt.GetScoreBreakdown();
		vector<float> ptScores = scores.GetScoresForProducer(pt);
		assert(ptScores.size() == 4);
		float pEF = ptScores[2];

		if (pEF > ret) {
			ret = pEF;
		}
	}

	return ret;
}

void LongerPhraseFF::EvaluateWhenApplied(const Hypothesis& hypo,
                                   ScoreComponentCollection* accumulator) const
{}

void LongerPhraseFF::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                        ScoreComponentCollection* accumulator) const
{}

void LongerPhraseFF::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "arg") {
    // set value here
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

}


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
  m_requireSortingAfterSourceContext = true;
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
    , TranslationOptionList &translationOptionList) const
{
  size_t inputSize = input.GetSize();

  float maxTransProb = GetMax(inputSize, range, transOptColl);

  TranslationOptionList::iterator iter;
  for (iter = translationOptionList.begin(); iter != translationOptionList.end(); ++iter) {
    TranslationOption &transOpt = **iter;
    ScoreComponentCollection &scores = transOpt.GetScoreBreakdown();
    scores.PlusEquals(this, maxTransProb);
  }
}

float LongerPhraseFF::GetMax(size_t inputSize, const WordsRange &range
                             ,const TranslationOptionCollection &transOptColl) const
{
  float ret = - std::numeric_limits<float>::infinity();
  size_t maxSizePhrase = StaticData::Instance().GetMaxPhraseLength();

  size_t startPos = range.GetStartPos();
  size_t endPos = range.GetEndPos();
  size_t width = range.GetNumWordsCovered();

  for (size_t currWidth = width + 1; currWidth <= inputSize; ++currWidth) {
    if (currWidth > maxSizePhrase) {
      break;
    }

    for (size_t currStartPos = 0; currStartPos <= startPos; ++currStartPos) {
      size_t currEndPos = currStartPos + currWidth - 1;

      if (currEndPos >= inputSize) {
        break;
      }
      if (currEndPos < endPos) {
        continue;
      }

      WordsRange range(currStartPos, currEndPos);

      const TranslationOptionList &currTransOpts = *transOptColl.GetTranslationOptionList(range);
      float currMax = GetMax(currTransOpts);
      if (currMax > ret) {
        ret = currMax;
      }
    }

  }

  if (ret == - std::numeric_limits<float>::infinity()) {
    ret = 0;
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


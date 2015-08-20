#include <vector>
#include "MadaConsistentTok.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"

using namespace std;

namespace Moses
{
int MadaConsistentTokState::Compare(const FFState& other) const
{
  const MadaConsistentTokState &otherState = static_cast<const MadaConsistentTokState&>(other);

  if (m_state == otherState.m_state)
    return 0;
  return (m_state < otherState.m_state) ? -1 : +1;
}

////////////////////////////////////////////////////////////////
MadaConsistentTok::MadaConsistentTok(const std::string &line)
  :StatefulFeatureFunction(3, line)
{
  ReadParameters();

  // init classes
  CLASSES["w+"] = 1;
  CLASSES["f+"] = 1;

  CLASSES["l+"] = 2;
  CLASSES["b+"] = 2;
  CLASSES["k+"] = 2;
  CLASSES["s+"] = 2;

  CLASSES["Al+"] = 3;

  CLASSES["+h"] = 5;
  CLASSES["+hA"] = 5;
  CLASSES["+hm"] = 5;
  CLASSES["+hmA"] = 5;
  CLASSES["+hn"] = 5;
  CLASSES["+k"] = 5;
  CLASSES["+km"] = 5;
  CLASSES["+kmA"] = 5;
  CLASSES["+kn"] = 5;
  CLASSES["+nA"] = 5;
  CLASSES["+y"] = 5;
}

void MadaConsistentTok::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void MadaConsistentTok::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void MadaConsistentTok::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* MadaConsistentTok::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  return new MadaConsistentTokState(0);
}

FFState* MadaConsistentTok::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new MadaConsistentTokState(0);
}

void MadaConsistentTok::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "arg") {
    // set value here
  } else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

}


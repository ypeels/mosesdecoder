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

  if (m_targetLen == otherState.m_targetLen)
    return 0;
  return (m_targetLen < otherState.m_targetLen) ? -1 : +1;
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
  // dense scores
  vector<float> newScores(m_numScoreComponents);
  newScores[0] = 1.5;
  newScores[1] = 0.3;
  newScores[2] = 0.4;
  accumulator->PlusEquals(this, newScores);

  // sparse scores
  accumulator->PlusEquals(this, "sparse-name", 2.4);

  // int targetLen = cur_hypo.GetCurrTargetPhrase().GetSize(); // ??? [UG]
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


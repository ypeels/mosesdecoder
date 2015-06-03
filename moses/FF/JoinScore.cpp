#include <vector>
#include "JoinScore.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"

using namespace std;

namespace Moses
{
int JoinScoreState::Compare(const FFState& other) const
{
  const JoinScoreState &otherState = static_cast<const JoinScoreState&>(other);

  if (m_targetLen == otherState.m_targetLen)
    return 0;
  return (m_targetLen < otherState.m_targetLen) ? -1 : +1;
}

////////////////////////////////////////////////////////////////
JoinScore::JoinScore(const std::string &line)
  :StatefulFeatureFunction(1, line)
{
  ReadParameters();
}

void JoinScore::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void JoinScore::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void JoinScore::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* JoinScore::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{

  // int targetLen = cur_hypo.GetCurrTargetPhrase().GetSize(); // ??? [UG]
  return new JoinScoreState(0);
}

FFState* JoinScore::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new JoinScoreState(0);
}

void JoinScore::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "arg") {
    // set value here
  } else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

}


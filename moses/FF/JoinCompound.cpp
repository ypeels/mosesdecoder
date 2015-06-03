#include <vector>
#include <boost/algorithm/string.hpp>
#include "JoinCompound.h"
#include "moses/ScoreComponentCollection.h"
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
  boost::replace_all(output, " +", "");
}

}


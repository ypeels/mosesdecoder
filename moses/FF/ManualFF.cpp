#include <vector>
#include "ManualFF.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"
#include "moses/InputFileStream.h"

using namespace std;

namespace Moses
{
int ManualFFState::Compare(const FFState& other) const
{
  return 0;
}

////////////////////////////////////////////////////////////////
ManualFF::ManualFF(const std::string &line)
  :StatefulFeatureFunction(1, line)
{
  ReadParameters();
}

void ManualFF::Load()
{
  InputFileStream inFile(m_filePath);

  size_t lineNum = 0;
  string line;
  while(getline(inFile, line)) {
    ++lineNum;
    vector<string> token = Tokenize<string>(line, " ");

  }
}

void ManualFF::EvaluateInIsolation(const Phrase &source
                                   , const TargetPhrase &targetPhrase
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection &estimatedFutureScore) const
{}

void ManualFF::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void ManualFF::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* ManualFF::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  // dense scores
  accumulator->PlusEquals(this, 4343.4);

  // int targetLen = cur_hypo.GetCurrTargetPhrase().GetSize(); // ??? [UG]
  return new ManualFFState(0);
}

FFState* ManualFF::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new ManualFFState(0);
}

void ManualFF::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "path") {
    m_filePath = value;
  } else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

}


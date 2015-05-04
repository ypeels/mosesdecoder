#include <vector>
#include "NonTermMinSpan.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/StackVec.h"
#include "moses/ChartCellLabel.h"

using namespace std;

namespace Moses
{
NonTermMinSpan::NonTermMinSpan(const std::string &line)
  :StatelessFeatureFunction(1, line)
  ,m_minSpan(2)
{
  m_tuneable = false;
  ReadParameters();
}

void NonTermMinSpan::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{
  targetPhrase.SetRuleSource(source);
}

void NonTermMinSpan::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{
  assert(stackVec);

  if (IsGlueRule(targetPhrase)) {
    return;
  }

  for (size_t ntInd = 0; ntInd < stackVec->size(); ++ntInd) {
    const ChartCellLabel &cell = *stackVec->at(ntInd);
    const WordsRange &range = cell.GetCoverage();

    if (range.GetNumWordsCovered() < m_minSpan) {
      if (m_tuneable) {
        scoreBreakdown.PlusEquals(this, 1);
      } else {
        scoreBreakdown.PlusEquals(this, - std::numeric_limits<float>::infinity());
        return;
      }
    }
  }
}

void NonTermMinSpan::EvaluateWhenApplied(const Hypothesis& hypo,
    ScoreComponentCollection* accumulator) const
{}

void NonTermMinSpan::EvaluateWhenApplied(const ChartHypothesis &hypo,
    ScoreComponentCollection* accumulator) const
{}

void NonTermMinSpan::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "min-span") {
    m_minSpan = Scan<size_t>(value);
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

std::vector<float> NonTermMinSpan::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 1, "Need 1 score");
  vector<float> ret(1, 1);
  return ret;
}

bool NonTermMinSpan::IsGlueRule(const TargetPhrase &targetPhrase) const
{
  const Phrase *source = targetPhrase.GetRuleSource();
  assert(source);

  string sourceStr = source->ToString();
  if (sourceStr == "<s> " || sourceStr == "X </s> " || sourceStr == "X X ") {
    // don't score glue rule
    //cerr << "sourceStr=" << sourceStr << endl;
    return true;
  } else {
    return false;
  }

}

}


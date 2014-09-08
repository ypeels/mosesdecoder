#include <vector>
#include "DiscardOnlyNonTermSpan.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/StackVec.h"
#include "moses/ChartCellLabel.h"
#include "moses/WordsRange.h"
#include "moses/PP/SpanLengthPhraseProperty.h"

using namespace std;

namespace Moses
{
DiscardOnlyNonTermSpan::DiscardOnlyNonTermSpan(const std::string &line)
  :StatelessFeatureFunction(1, line)
  ,m_span(1)
{
  m_tuneable = false;
  ReadParameters();
}

void DiscardOnlyNonTermSpan::EvaluateInIsolation(const Phrase &source
                                   , const TargetPhrase &targetPhrase
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection &estimatedFutureScore) const
{
}

void DiscardOnlyNonTermSpan::EvaluateWithSourceContext(const InputType &input
                                   , const InputPath &inputPath
                                   , const TargetPhrase &targetPhrase
                                   , const StackVec *stackVec
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection *estimatedFutureScore) const
{
	  assert(stackVec);

	  const PhraseProperty *property = targetPhrase.GetProperty("SpanLength");
	  if (property == NULL) {
		  return;
	  }
	  const SpanLengthPhraseProperty *slProp = static_cast<const SpanLengthPhraseProperty*>(property);

	  for (size_t i = 0; i < stackVec->size(); ++i) {
		  const ChartCellLabel &cell = *stackVec->at(i);
		  const WordsRange &ntRange = cell.GetCoverage();
		  size_t sourceWidth = ntRange.GetNumWordsCovered();
		  bool onlyContainSpanLength = slProp->OnlyContainSpanLength(i, sourceWidth);

		  if (onlyContainSpanLength) {
			  float score = - std::numeric_limits<float>::infinity();
			  scoreBreakdown.PlusEquals(this, score);
			  return;
		  }
	  }

}

void DiscardOnlyNonTermSpan::EvaluateWhenApplied(const Hypothesis& hypo,
                                   ScoreComponentCollection* accumulator) const
{}

void DiscardOnlyNonTermSpan::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                        ScoreComponentCollection* accumulator) const
{}

void DiscardOnlyNonTermSpan::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "span") {
	  m_span = Scan<size_t>(value);
  }
  else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

}


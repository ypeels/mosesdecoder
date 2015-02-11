#include <vector>
#include "DiscardLeftRightNonTerm.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/StackVec.h"
#include "moses/ChartCellLabel.h"
#include "moses/WordsRange.h"
#include "moses/PP/SpanLengthPhraseProperty.h"

using namespace std;

namespace Moses
{
DiscardLeftRightNonTerm::DiscardLeftRightNonTerm(const std::string &line)
  :StatelessFeatureFunction(1, line)
  ,m_hardConstraint(true)
  ,m_doLeft(false)
  ,m_doRight(false)
  ,m_doMiddle(false)
  ,m_useTarget(false)
{
  m_tuneable = false;
  ReadParameters();
}

void DiscardLeftRightNonTerm::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{
  if (IsGlueRule(source)) {
	return;
  }

  const Phrase *phrase = m_useTarget ? &targetPhrase : &source;

  bool left = false, right = false, middle = false;

  if (phrase->Front().IsNonTerminal()) {
	  left = true;
  }
  if (phrase->Back().IsNonTerminal()) {
	  right = true;
  }

  for (size_t i = 1; i < phrase->GetSize() - 1; ++i) {
	const Word &word = phrase->GetWord(i);
	if (word.IsNonTerminal()) {
		middle = true;
		break;
	}
  }

  if (m_hardConstraint) {
	  UTIL_THROW_IF2(m_numScoreComponents != 1, "Must have 1 score");

	  if ((m_doLeft && left) ||
		  (m_doRight && right) ||
		  (m_doMiddle && middle)) {
	      scoreBreakdown.PlusEquals(this, - std::numeric_limits<float>::infinity());
	  }
  }
  else {
	  vector<float> scores(m_numScoreComponents, 0);
	  size_t scoreInd = 0;
	  if (m_doLeft) {
		  if (left) {
			  scores[scoreInd] = 1;
			  ++scoreInd;
		  }
	  }

	  if (m_doRight) {
		  if (right) {
			  scores[scoreInd] = 1;
			  ++scoreInd;
		  }
	  }

	  if (m_doMiddle) {
		  if (middle) {
			  scores[scoreInd] = 1;
			  ++scoreInd;
		  }
	  }

      scoreBreakdown.PlusEquals(this, scores);
  }
}

void DiscardLeftRightNonTerm::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void DiscardLeftRightNonTerm::EvaluateWhenApplied(const Hypothesis& hypo,
    ScoreComponentCollection* accumulator) const
{}

void DiscardLeftRightNonTerm::EvaluateWhenApplied(const ChartHypothesis &hypo,
    ScoreComponentCollection* accumulator) const
{}

void DiscardLeftRightNonTerm::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "left") {
	  m_doLeft = Scan<bool>(value);
  }
  else if (key == "right") {
	  m_doRight = Scan<bool>(value);
  }
  else if (key == "middle") {
	  m_doMiddle = Scan<bool>(value);
  }
  else if (key == "hard-constraint") {
    m_hardConstraint = Scan<bool>(value);
  }
  else if (key == "use-target") {
	  m_useTarget = Scan<bool>(value);
  }
  else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

std::vector<float> DiscardLeftRightNonTerm::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 1,
                 "DiscardLeftRightNonTerm must only have 1 score");
  vector<float> ret(1, 1);
  return ret;
}

bool DiscardLeftRightNonTerm::IsGlueRule(const Phrase &source) const
{
  string sourceStr = source.ToString();
  if (sourceStr == "<s> " || sourceStr == "X </s> " || sourceStr == "X X ") {
    // don't score glue rule
    //cerr << "sourceStr=" << sourceStr << endl;
    return true;
  } else {
    return false;
  }

}

} // namespace


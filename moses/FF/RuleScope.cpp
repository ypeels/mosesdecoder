#include "RuleScope.h"
#include "moses/StaticData.h"
#include "moses/Word.h"

namespace Moses
{
RuleScope::RuleScope(const std::string &line)
  :StatelessFeatureFunction(1, line)
  ,m_sourceSyntax(true)
{
}

bool IsAmbiguous(const Word &word, bool sourceSyntax)
{
  const Word &inputDefaultNonTerminal = StaticData::Instance().GetInputDefaultNonTerminal();
  return word.IsNonTerminal() && (!sourceSyntax || word == inputDefaultNonTerminal);
}

void RuleScope::EvaluateInIsolation(const Phrase &source
                                    , const TargetPhrase &targetPhrase
                                    , ScoreComponentCollection &scoreBreakdown
                                    , ScoreComponentCollection &estimatedFutureScore) const
{
  if (IsGlueRule(source)) {
	return;
  }

  float score = 0;

  if (source.GetSize() > 0 && source.Front().IsNonTerminal()) {
	++score;
  }
  if (source.GetSize() > 1 && source.Back().IsNonTerminal()) {
	++score;
  }

  /*
  int count = 0;
  for (size_t i = 0; i < source.GetSize(); ++i) {
    const Word &word = source.GetWord(i);
    bool ambiguous = IsAmbiguous(word, m_sourceSyntax);
    if (ambiguous) {
      ++count;
    } else {
      if (count > 0) {
        score += count;
      }
      count = -1;
    }
  }

  // 1st & last always adjacent to ambiguity
  ++count;
  if (count > 0) {
    score += count;
  }
  */

  scoreBreakdown.PlusEquals(this, score);
}

void RuleScope::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "source-syntax") {
    m_sourceSyntax = Scan<bool>(value);
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
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

}


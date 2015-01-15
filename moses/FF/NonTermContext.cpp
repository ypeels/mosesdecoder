#include <vector>
#include "NonTermContext.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/StackVec.h"
#include "moses/ChartCellLabel.h"
#include "moses/InputType.h"
#include "moses/PP/NonTermContextProperty.h"

using namespace std;

namespace Moses
{
NonTermContext::NonTermContext(const std::string &line)
  :StatelessFeatureFunction(2, line)
  ,m_smoothConst(1)
  ,m_factor(0)
  ,m_type(0)
  ,m_doInner(true)
  ,m_doOuter(true)
  ,m_onlyAmbiguousNT(false)
{
  m_requireSortingAfterSourceContext = true;
  ReadParameters();
}

void NonTermContext::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{
  if (m_onlyAmbiguousNT) {
    targetPhrase.SetRuleSource(source);
  }
}

void NonTermContext::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{
  assert(stackVec);

  const PhraseProperty *prop = targetPhrase.GetProperty("NonTermContext");
  if (prop == NULL) {
    return;
  }
  const NonTermContextProperty &ntContextProp = *static_cast<const NonTermContextProperty*>(prop);

  if (m_onlyAmbiguousNT) {
    const Phrase *sourcePhrase = targetPhrase.GetRuleSource();
    assert(sourcePhrase);

    size_t ntInd = 0;
    for (size_t sourcePos = 0; sourcePos < sourcePhrase->GetSize(); ++sourcePos) {
      const Word &word = sourcePhrase->GetWord(sourcePos);
      if (word.IsNonTerminal()) {
        bool ambiguous = false;

        if (sourcePos == 0 || sourcePos + 1 == sourcePhrase->GetSize()) {
          ambiguous = true;
        } else if (sourcePhrase->GetWord(sourcePos - 1).IsNonTerminal()
                   || sourcePhrase->GetWord(sourcePos + 1).IsNonTerminal()) {
          ambiguous = true;
        }

        // NT could vary in length
        if (ambiguous) {
          const ChartCellLabel &cell = *stackVec->at(ntInd);
          SetScores(ntInd, input, ntContextProp, cell, targetPhrase, scoreBreakdown);
        }

        ++ntInd;
      }

    }
  } else {
    for (size_t ntInd = 0; ntInd < stackVec->size(); ++ntInd) {
      const ChartCellLabel &cell = *stackVec->at(ntInd);
      SetScores(ntInd, input, ntContextProp, cell, targetPhrase, scoreBreakdown);
    }
  }
}

void NonTermContext::EvaluateWhenApplied(const Hypothesis& hypo,
    ScoreComponentCollection* accumulator) const
{}

void NonTermContext::EvaluateWhenApplied(const ChartHypothesis &hypo,
    ScoreComponentCollection* accumulator) const
{}

void NonTermContext::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "constant") {
    m_smoothConst = Scan<float>(value);
  } else if (key == "do-inner") {
    m_doInner = Scan<bool>(value);
  } else if (key == "do-outer") {
    m_doOuter = Scan<bool>(value);
  } else if (key == "only-ambiguous-nt") {
    m_onlyAmbiguousNT = Scan<bool>(value);
  } else if (key == "factor") {
    m_factor = Scan<FactorType>(value);
  } else if (key == "type") {
    if (value == "independent") {
      m_type = 0;
    } else if (value == "joint") {
      m_type = 1;
    } else {
      UTIL_THROW(util::Exception, "Unknown type: " << value);
    }
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

void NonTermContext::SetScores(size_t ntInd, const InputType &input,
                               const NonTermContextProperty &ntContextProp,
                               const ChartCellLabel &cell,
                               const TargetPhrase &targetPhrase,
                               ScoreComponentCollection &scoreBreakdown) const
{
  const WordsRange &range = cell.GetCoverage();

  const Word *leftOuter, *rightOuter, *leftInner, *rightInner;
  if (m_doOuter) {
    leftOuter = &input.GetWord(range.GetStartPos() - 1);
    rightOuter = &input.GetWord(range.GetEndPos() + 1);
  }
  if (m_doInner) {
    leftInner = &input.GetWord(range.GetStartPos());
    rightInner = &input.GetWord(range.GetEndPos());
  }

  vector<float> scores(m_numScoreComponents);
  size_t scoreInd = 0;

  if (m_type == 0) {
    //cerr << "if (m_type == 0) {" << endl;
    if (m_doOuter) {
      float outer = ntContextProp.GetProb(ntInd, 0, leftOuter->GetFactor(m_factor), m_smoothConst);
      outer *= ntContextProp.GetProb(ntInd, 3, rightOuter->GetFactor(m_factor), m_smoothConst);
      scores[scoreInd] = TransformScore(outer);
      ++scoreInd;
    }

    if (m_doInner) {
      float inner = ntContextProp.GetProb(ntInd, 1, leftInner->GetFactor(m_factor), m_smoothConst);
      inner *= ntContextProp.GetProb(ntInd, 2, rightInner->GetFactor(m_factor), m_smoothConst);
      scores[scoreInd] = TransformScore(inner);
      ++scoreInd;
    }
  } else if (m_type == 1) {
    //cerr << "if (m_type == 1) {" << endl;
    if (m_doOuter) {
      float outer = ntContextProp.GetProb(ntInd,
                                          1,
                                          leftOuter->GetFactor(m_factor),
                                          rightOuter->GetFactor(m_factor),
                                          m_smoothConst);
      scores[scoreInd] = TransformScore(outer);
      ++scoreInd;
    }

    if (m_doInner) {
      float inner = ntContextProp.GetProb(ntInd,
                                          0,
                                          leftInner->GetFactor(m_factor),
                                          rightInner->GetFactor(m_factor),
                                          m_smoothConst);
      scores[scoreInd] = TransformScore(inner);
      ++scoreInd;
    }
  }

  // all done. set score
  scoreBreakdown.PlusEquals(this, scores);
}

}


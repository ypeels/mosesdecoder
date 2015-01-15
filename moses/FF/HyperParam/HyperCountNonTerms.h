#pragma once

#include "moses/FF/StatelessFeatureFunction.h"

namespace Moses
{
class PhraseDictionary;

class HyperCountNonTerms : public StatelessFeatureFunction
{
public:
  HyperCountNonTerms(const std::string &line);
  bool IsUseable(const FactorMask &mask) const {
    return true;
  }

  void EvaluateInIsolation(const Phrase &source
                           , const TargetPhrase &targetPhrase
                           , ScoreComponentCollection &scoreBreakdown
                           , ScoreComponentCollection &estimatedFutureScore) const
  {}

  void EvaluateWithSourceContext(const InputType &input
                                 , const InputPath &inputPath
                                 , const TargetPhrase &targetPhrase
                                 , const StackVec *stackVec
                                 , ScoreComponentCollection &scoreBreakdown
                                 , ScoreComponentCollection *estimatedFutureScore = NULL) const
  {}

  void EvaluateWhenApplied(const Hypothesis& hypo,
                           ScoreComponentCollection* accumulator) const
  {}

  void EvaluateWhenApplied(
    const ChartHypothesis& hypo,
    ScoreComponentCollection* accumulator) const
  {}

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const
  {}

  void EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList) const;

  void SetParameter(const std::string& key, const std::string& value);

protected:
  const PhraseDictionary *m_pt;
};

}


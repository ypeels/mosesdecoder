#pragma once

#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"

namespace Moses
{

class LM01State : public FFState
{
	const Factor *m_factor;
public:
  LM01State(const Factor *factor)
    :m_factor(factor)
 {}

  int Compare(const FFState& other) const
  {
    const LM01State &otherState = static_cast<const LM01State&>(other);

    if (m_factor == otherState.m_factor)
      return 0;
    return (m_factor < otherState.m_factor) ? -1 : +1;
  }

  const Factor *GetFactor() const
  { return m_factor; }
};

class LM01 : public StatefulFeatureFunction
{
public:
  LM01(const std::string &line);

  void Load();

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }
  virtual const FFState* EmptyHypothesisState(const InputType &input) const {
    return new LM01State(0);
  }

  void EvaluateInIsolation(const Phrase &source
                           , const TargetPhrase &targetPhrase
                           , ScoreComponentCollection &scoreBreakdown
                           , ScoreComponentCollection &estimatedFutureScore) const;
  void EvaluateWithSourceContext(const InputType &input
                                 , const InputPath &inputPath
                                 , const TargetPhrase &targetPhrase
                                 , const StackVec *stackVec
                                 , ScoreComponentCollection &scoreBreakdown
                                 , ScoreComponentCollection *estimatedFutureScore = NULL) const;

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const;

  FFState* EvaluateWhenApplied(
    const Hypothesis& cur_hypo,
    const FFState* prev_state,
    ScoreComponentCollection* accumulator) const;
  FFState* EvaluateWhenApplied(
    const ChartHypothesis& /* cur_hypo */,
    int /* featureID - used to index the state in the previous hypotheses */,
    ScoreComponentCollection* accumulator) const;

  void SetParameter(const std::string& key, const std::string& value);
  std::vector<float> DefaultWeights() const;

protected:
  FactorType m_beginFactor, m_endFactor;
  bool m_processUnk;
  float m_minCount;
  std::string m_filePath;

  typedef std::pair<FactorType, const Factor*> VocabKey;
  std::set<VocabKey> m_vocab;

  // LM data
  std::map<size_t, float> m_data;

  float GetCount(const Factor *prevFactor, const Factor *currFactor) const;
  size_t GetHash(const Factor *factor1, const Factor *factor2) const;

  bool InVocab(const VocabKey &key) const;
};


}


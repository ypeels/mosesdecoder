#pragma once

#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"

namespace Moses
{

class MorphoLMState : public FFState
{
  Phrase m_lastWords;
public:
  MorphoLMState(const Phrase &context)
    :m_lastWords(context) {
  }

  int Compare(const FFState& other) const;
  FFState *Clone() const {
    abort();
    return NULL;
  }

  const Phrase &GetPhrase() const
  { return m_lastWords; }
};

class MorphoLM : public StatefulFeatureFunction
{
protected:
	std::string m_path;
	size_t m_order;
    FactorType	m_factorType;

    Word m_sentenceStartWord, m_sentenceEndWord; //! Contains factors which represents the beging and end words for this LM.

public:
  MorphoLM(const std::string &line);

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }

  virtual const FFState* EmptyHypothesisState(const InputType &input) const;

  virtual void Load();

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

};


}


#pragma once

#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"

namespace Moses
{

class LM01State : public FFState
{
  size_t m_id;
public:
  LM01State(size_t id)
    :m_id(id)
 {}

  int Compare(const FFState& other) const;

  size_t GetId() const
  { return m_id; }
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

  // vocab
  //typedef std::pair<size_t, int> VocabValue; // 1st = id, 2nd = hash
  //std::map<const Factor*, VocabValue> m_word2id;
  std::map<const Factor*, size_t> m_word2id;
  	  // id = 0 - UNK

  // LM data
  std::map<size_t, float> m_data;

  size_t GetVocab(const Word &word, FactorType factorType) const;
  float GetCount(size_t id, size_t prevId) const;

};


}


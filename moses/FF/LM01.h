#pragma once

#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"

namespace Moses
{

class LM01State : public FFState
{
public:
  Word m_word;

  LM01State()
  {}

  LM01State(Word &word)
    :m_word(word)
  {}

  int Compare(const FFState& other) const {
    const LM01State &otherState = static_cast<const LM01State&>(other);
    int ret = Word::Compare(m_word, otherState.m_word);
    return ret;
  }
  FFState *Clone() const {
    abort();
    return NULL;
  }

};

class LM01 : public StatefulFeatureFunction
{
  struct Config {
    FactorType beginFactor, endFactor;
    float minCount;

    Config(FactorType beginFactor, FactorType endFactor, float minCount) {
      this->beginFactor = beginFactor;
      this->endFactor = endFactor;
      this->minCount = minCount;
    }
  };

public:
  LM01(const std::string &line);

  void Load();

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }
  virtual const FFState* EmptyHypothesisState(const InputType &input) const {
    return new LM01State();
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
  std::vector<Config> m_configs;
  bool m_processUnk;
  std::string m_filePath;

  typedef std::pair<FactorType, const Factor*> VocabKey;
  std::set<VocabKey> m_vocab;

  // LM data
  std::map<size_t, float> m_data;

  void AddCount(const Config &config, const Factor *factor1, const Factor *factor2, float count);

  float GetCount(const Config &config, const Factor *prevFactor, const Factor *currFactor) const;
  size_t GetHash(const Config &config, const Factor *factor1, const Factor *factor2) const;

  bool InVocab(const VocabKey &key) const;

  void ParseConfig(const std::string &value);
  void Parse1Config(const std::string &value);
};


}


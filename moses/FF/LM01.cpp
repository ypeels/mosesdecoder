#include <vector>
#include <boost/functional/hash.hpp>
#include "LM01.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"
#include "moses/FactorCollection.h"
#include "moses/InputFileStream.h"
#include "moses/TranslationOption.h"
#include "moses/TargetPhrase.h"

using namespace std;

namespace Moses
{
int LM01State::Compare(const FFState& other) const
{
  const LM01State &otherState = static_cast<const LM01State&>(other);

  if (m_id == otherState.m_id)
    return 0;
  return (m_id < otherState.m_id) ? -1 : +1;
}

////////////////////////////////////////////////////////////////
LM01::LM01(const std::string &line)
  :StatefulFeatureFunction(1, line)
  ,m_beginFactor(0)
  ,m_endFactor(0)
  ,m_processUnk(false)
  ,m_minCount(1)
{
  m_tuneable = false;
  ReadParameters();

  UTIL_THROW_IF2(m_filePath.empty(), "Must specify data directory");
}

std::vector<float> LM01::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 1,
                 "LM01 must only have 1 score");
  vector<float> ret(1, 1);
  return ret;
}

void LM01::Load()
{
  FactorCollection &factorCollection = FactorCollection::Instance();

  string line;

  string vocabPath = m_filePath + "/Vocab.dat";
  InputFileStream vocabStrme(vocabPath);

  while (getline(vocabStrme, line)) {
	  vector<string> toks;
	  Tokenize(toks, line);
	  UTIL_THROW_IF2(toks.size() != 2, "Expected 2 columns in vocab file");

	  const Factor *factor = factorCollection.AddFactor(toks[0]);
	  string str = factor->GetString().as_string();

	  size_t id = Scan<size_t>(toks[1]);

	  m_word2id[factor] = id;
  }

  string dataPath = m_filePath + "/Data.dat";
  InputFileStream dataStrme(dataPath);

  while (getline(dataStrme, line)) {
	  vector<string> toks;
	  Tokenize(toks, line);
	  UTIL_THROW_IF2(toks.size() != 2, "Expected 2 columns in data file");

	  size_t hash = Scan<size_t>(toks[0]);
	  float count = Scan<float>(toks[1]);

	  m_data[hash] = count;
  }

}

void LM01::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void LM01::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void LM01::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* LM01::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  const TargetPhrase &tp = cur_hypo.GetTranslationOption().GetTargetPhrase();
  if (tp.GetSize() == 0) {
	  // initial trans opt. 0 size tp
	  return new LM01State(0);
  }

  size_t id;

  // 1st word from this hypo, last word from previous
  const Word &firstWord = tp.Front();
  id = GetVocab(firstWord, m_endFactor);

  const LM01State *prevLMState = static_cast<const LM01State*>(prev_state);
  size_t prevId = prevLMState->GetId();

  if ((id && prevId) || m_processUnk) {
	  float count = GetCount(id, prevId);
	  if (count < m_minCount) {
		  accumulator->PlusEquals(this, - std::numeric_limits<float>::infinity());
	  }
  }

  // state info
  const Word &lastWord = tp.Back();
  id = GetVocab(lastWord, m_beginFactor);
  return new LM01State(id);
}

FFState* LM01::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new LM01State(0);
}

size_t LM01::GetVocab(const Word &word, FactorType factorType) const
{
  const Factor *factor = word.GetFactor(factorType);

  std::map<const Factor*, size_t>::const_iterator iter;
  iter = m_word2id.find(factor);

  if (iter == m_word2id.end()) {
	  return 0;
  }
  else {
	  return iter->second;
  }
}

float LM01::GetCount(size_t id, size_t prevId) const
{
  size_t seed = id;
  boost::hash_combine(seed, prevId);

  std::map<size_t, float>::const_iterator iter;
  iter = m_data.find(seed);
  if (iter == m_data.end()) {
	  return 0;
  }
  else {
	  return iter->second;
  }
}

void LM01::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "begin-factor") {
	  m_beginFactor = Scan<size_t>(value);
  }
  else if (key == "end-factor") {
	  m_endFactor = Scan<size_t>(value);
  }
  else if (key == "path") {
	  m_filePath = value;
  }
  else if (key == "min-count") {
	  m_minCount = Scan<float>(value);
   }
  else if (key == "process-unk") {
	  m_processUnk = Scan<bool>(value);
   }
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

}


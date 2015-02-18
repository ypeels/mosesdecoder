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

  UTIL_THROW_IF2(m_filePath.empty(), "Must specify file path");
}

std::vector<float> LM01::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 1,
                 "LM01 must only have 1 score");
  vector<float> ret(1, - std::numeric_limits<float>::infinity());
  return ret;
}

void LM01::Load()
{
  FactorCollection &factorCollection = FactorCollection::Instance();

  InputFileStream dataStrme(m_filePath);

  string line;
  while (getline(dataStrme, line)) {
	  vector<string> toks;
	  Tokenize(toks, line);
	  UTIL_THROW_IF2(toks.size() != 3, "Expected 3 columns in data file");

	  float count = Scan<float>(toks[0]);
	  const Factor *factor1 = factorCollection.AddFactor(toks[1]);
	  const Factor *factor2 = factorCollection.AddFactor(toks[2]);

	  VocabKey vocabKey1(m_beginFactor, factor1);
	  m_vocab.insert(vocabKey1);
	  VocabKey vocabKey2(m_endFactor, factor2);
	  m_vocab.insert(vocabKey2);

	  size_t hash = GetHash(factor1, factor2);
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
	  return new LM01State(NULL);
  }

  // 1st word from this hypo, last word from previous
  const Word &firstWord = tp.Front();
  const Factor *firstFactor = firstWord[m_endFactor];

  const LM01State *prevLMState = static_cast<const LM01State*>(prev_state);
  const Factor *prevFactor = prevLMState->GetFactor();

  if (m_processUnk || (InVocab(VocabKey(m_beginFactor, prevFactor))
		  	  	  	  && InVocab(VocabKey(m_endFactor, firstFactor)))) {
	  float count = GetCount(prevFactor, firstFactor);
	  if (count < m_minCount) {
		  accumulator->PlusEquals(this, 1);
	  }
  }

  // state info
  const Word &lastWord = tp.Back();
  const Factor *lastFactor = lastWord[m_beginFactor];
  return new LM01State(lastFactor);
}

FFState* LM01::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new LM01State(0);
}

float LM01::GetCount(const Factor *prevFactor, const Factor *currFactor) const
{
  size_t hash = GetHash(prevFactor, currFactor);

  std::map<size_t, float>::const_iterator iter;
  iter = m_data.find(hash);
  if (iter == m_data.end()) {
	  return 0;
  }
  else {
	  return iter->second;
  }
}

size_t LM01::GetHash(const Factor *factor1, const Factor *factor2) const
{
  size_t hash = (size_t)factor1;
  boost::hash_combine(hash, (size_t)factor2);
  return hash;
}

bool LM01::InVocab(const VocabKey &key) const
{
	std::set<VocabKey>::const_iterator iter;
	iter = m_vocab.find(key);
	return (iter != m_vocab.end());
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


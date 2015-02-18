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
  ,m_processUnk(false)
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

	  vector<string> str1 = Tokenize(toks[1], "|");
	  vector<string> str2 = Tokenize(toks[2], "|");

	  for (size_t i = 0; i < m_configs.size(); ++i) {
		  const Config &config = m_configs[i];

		  float count = Scan<float>(toks[0]);
		  const Factor *factor1 = factorCollection.AddFactor(str1[config.beginFactor]);
		  const Factor *factor2 = factorCollection.AddFactor(str2[config.endFactor]);

		  VocabKey vocabKey1(config.beginFactor, factor1);
		  m_vocab.insert(vocabKey1);
		  VocabKey vocabKey2(config.endFactor, factor2);
		  m_vocab.insert(vocabKey2);

		  AddCount(config, factor1, factor2, count);
	  }
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
	  return new LM01State();
  }

  const LM01State *prevLMState = static_cast<const LM01State*>(prev_state);

  // 1st word from this hypo, last word from previous
  const Word &firstWord = tp.Front();

  bool doIt = true;
  for (size_t i = 0; i < m_configs.size(); ++i) {
	  const Config &config = m_configs[i];

	  const Factor *firstFactor = firstWord[config.endFactor];
	  const Factor *prevFactor = prevLMState->m_word[config.beginFactor];

	  if (!m_processUnk && (!InVocab(VocabKey(config.beginFactor, prevFactor))
				|| !InVocab(VocabKey(config.endFactor, firstFactor)))) {
	    doIt = false;
	    break;
	  }

	  float count = GetCount(config, prevFactor, firstFactor);
	  if (count > config.minCount) {
		  doIt = false;
		  break;
	  }
  }

  if (doIt) {
	  accumulator->PlusEquals(this, 1);
	  cerr << "Get rid:" << prevLMState->m_word << " " << firstWord << endl;
  }

  // state info
  Word newStateWord;
  const Word &lastWord = tp.Back();
  for (size_t i = 0; i < m_configs.size(); ++i) {
	  const Config &config = m_configs[i];
	  newStateWord[config.beginFactor] = lastWord[config.beginFactor];
  }
  return new LM01State(newStateWord);
}

FFState* LM01::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new LM01State();
}

void LM01::AddCount(const Config &config, const Factor *factor1, const Factor *factor2, float count)
{
  size_t hash = GetHash(config, factor1, factor2);
  std::map<size_t, float>::iterator iter;

  float total = 0;
  iter = m_data.find(hash);
  if (iter != m_data.end()) {
	  total = iter->second;
  }
  total += count;
  m_data[hash] = total;
}

float LM01::GetCount(const Config &config, const Factor *prevFactor, const Factor *currFactor) const
{
  size_t hash = GetHash(config, prevFactor, currFactor);

  std::map<size_t, float>::const_iterator iter;
  iter = m_data.find(hash);
  if (iter == m_data.end()) {
	  return 0;
  }
  else {
	  return iter->second;
  }
}

size_t LM01::GetHash(const Config &config, const Factor *factor1, const Factor *factor2) const
{
  size_t hash = (size_t) &config;
  boost::hash_combine(hash, (size_t)factor1);
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
  if (key == "config") {
	  // format: sourceFactors-targetFactors-minCount:...
	  // eg 0-0-1:0-1-1:1-0-1
	  ParseConfig(value);
  }
  else if (key == "path") {
	  m_filePath = value;
  }
  else if (key == "process-unk") {
	  m_processUnk = Scan<bool>(value);
  }
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

void LM01::ParseConfig(const std::string &value)
{
	vector<string> toks = Tokenize(value, ":");
	for (size_t i = 0; i < toks.size(); ++i) {
		string &tok = toks[i];
		Parse1Config(tok);
	}
}

void LM01::Parse1Config(const std::string &value)
{
  vector<string> toks = Tokenize(value, "-");
  UTIL_THROW_IF2(toks.size() != 3, "wrong format");

  Config config(Scan<size_t>(toks[0]),
		  Scan<size_t>(toks[1]),
		  Scan<float>(toks[2]) );
  m_configs.push_back(config);
}
}


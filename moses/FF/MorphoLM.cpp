#include <vector>
#include "MorphoLM.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"
#include "moses/FactorCollection.h"

using namespace std;

namespace Moses
{
int MorphoLMState::Compare(const FFState& other) const
{
  const MorphoLMState &otherState = static_cast<const MorphoLMState&>(other);
  int ret = m_lastWords.Compare(otherState.m_lastWords);

  return ret;
}

////////////////////////////////////////////////////////////////
MorphoLM::MorphoLM(const std::string &line)
:StatefulFeatureFunction(1, line)
,m_factorType(0)
{
  ReadParameters();

  FactorCollection &fc = FactorCollection::Instance();
  const Factor *startFactor = fc.AddFactor("<s>", false);
  m_sentenceStartWord.SetFactor(m_factorType, startFactor);

  const Factor *endFactor = fc.AddFactor("</s>", false);
  m_sentenceEndWord.SetFactor(m_factorType, endFactor);
}

const FFState* MorphoLM::EmptyHypothesisState(const InputType &input) const {
  Phrase phrase;
  phrase.AddWord(m_sentenceStartWord);

  return new MorphoLMState(phrase);
}

void MorphoLM::Load()
{
  // load(path);
}

void MorphoLM::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void MorphoLM::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void MorphoLM::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* MorphoLM::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  // dense scores
  float score = 0;
  size_t targetLen = cur_hypo.GetCurrTargetPhrase().GetSize();
  const WordsRange &targetRange = cur_hypo.GetCurrTargetWordsRange();

  Phrase context;

  if (prev_state) {
	  const MorphoLMState *prevMorphState = static_cast<const MorphoLMState*>(prev_state);
	  context.Append(prevMorphState->GetPhrase());
  }

  for (size_t pos = targetRange.GetStartPos(); pos < targetLen; ++pos){
	  const Word &word = cur_hypo.GetWord(pos);
	  context.AddWord(word);
	  // score

  }

  // finished scoring. set score
  accumulator->PlusEquals(this, score);


  return new MorphoLMState(context);
}

FFState* MorphoLM::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  abort();
  return NULL;
}

void MorphoLM::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "path") {
    m_path = value;
  }
  else if (key == "order") {
    m_order = Scan<size_t>(value);
  }
  else if (key == "factor") {
	m_factorType = Scan<FactorType>(value);
  }
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

}


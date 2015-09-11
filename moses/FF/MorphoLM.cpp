#include <vector>
#include "MorphoLM.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"
#include "moses/FactorCollection.h"
#include "moses/InputFileStream.h"
#include "util/exception.hh"

#include "moses/FF/JoinScore/TrieSearch.h"
#include "moses/FF/MorphoTrie/MorphTrie.h"

using namespace std;

namespace Moses
{
int MorphoLMState::Compare(const FFState& other) const
{
  const MorphoLMState &otherState = static_cast<const MorphoLMState&>(other);

  if (m_lastWords < otherState.m_lastWords) {
	  return -1;
  }
  else if (m_lastWords > otherState.m_lastWords) {
	  return +1;
  }

  // context words equal. Compare last unfinished word
  if (m_unfinishedWord < otherState.m_unfinishedWord) {
	  return -1;
  }
  else if (m_unfinishedWord > otherState.m_unfinishedWord) {
	  return +1;
  }

  return 0;
}

////////////////////////////////////////////////////////////////
MorphoLM::MorphoLM(const std::string &line)
:StatefulFeatureFunction(1, line)
,m_order(0)
,m_factorType(0)
,m_marker("+")
,m_binLM(false)
{
  ReadParameters();

  if (m_order == 0) {
	UTIL_THROW2("Must set order");
  }

  FactorCollection &fc = FactorCollection::Instance();
  m_sentenceStart = fc.AddFactor("<s>", false);
  m_sentenceEnd = fc.AddFactor("</s>", false);
}

const FFState* MorphoLM::EmptyHypothesisState(const InputType &input) const {
  std::vector<const Factor*> context;
  context.push_back(m_sentenceStart);

  return new MorphoLMState(context, "", 0.0);
}

void MorphoLM::Load()
{
  if (m_binLM) {
	  m_trieSearch = new TrieSearch<LMScores, NGRAM>;
	  m_trieSearch->Create(m_path);

	  // vocab
	  FactorCollection &fc = FactorCollection::Instance();

	  m_vocab = new std::map<const Factor *, uint64_t>;

	  InputFileStream vocabStrme(m_path + ".vocab");
	  string line;
	  while (getline(vocabStrme, line)) {
		  vector<string> toks = Tokenize(line);
		  assert(toks.size() == 2);

		  const Factor *factor = fc.AddFactor(toks[0], false);
		  uint64_t vocabId = Scan<uint64_t>(toks[1]);
		  (*m_vocab)[factor] = vocabId;
	  }
  }
  else {
	  FactorCollection &fc = FactorCollection::Instance();
	  root = new MorphTrie<const Factor*, float>;

	  InputFileStream infile(m_path);
	  string line;
	  while (getline(infile, line)) {
		  vector<string> substrings;
		  Tokenize(substrings, line, "\t");

		  if (substrings.size() < 2)
			   continue;

		  float weight = Moses::Scan<float>(substrings[0]);
		  if (substrings[1] == "<unk>") {
			  m_oov = weight;
			  continue;
		  }

		  vector<string> key;
		  Tokenize(key, substrings[1], " ");

		  vector<const Factor*> factorKey;
		  for (int i = 0; i < key.size(); ++i)
			  factorKey.push_back(fc.AddFactor(key[i], false));

		  float backoff = 0.f;
		  if (substrings.size() == 3)
			backoff = Moses::Scan<float>(substrings[2]);


	   //reverse(key.begin(), key.end());
		  root->insert(factorKey, weight, backoff);
	  }

	  //LoadLm(m_path, root, m_oov);
  }
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
  float prev_score = 0.0;
  float kn_score = 0.0;
  size_t targetLen = cur_hypo.GetCurrTargetPhrase().GetSize();
  const WordsRange &targetRange = cur_hypo.GetCurrTargetWordsRange();

  assert(prev_state);

  const MorphoLMState *prevMorphState = static_cast<const MorphoLMState*>(prev_state);

  bool prevIsMorph = prevMorphState->GetPrevIsMorph();
  string prevMorph = prevMorphState->GetPrevMorph();
  prev_score = prevMorphState->GetPrevScore();

  vector<const Factor*> context = prevMorphState->GetPhrase();



  //vector<string> stringContext;
  //SetContext(stringContext, prevMorphState->GetPhrase());
  FactorCollection &fc = FactorCollection::Instance();
  for (size_t pos = 0; pos < targetLen; ++pos){
	  const Word &word = cur_hypo.GetCurrWord(pos);
	  const Factor *factor = word[m_factorType];
	  string str = factor->GetString().as_string();

	  if (str.size() == 1 && str == "+") {
	  		// do nothing
      }
      else if (str[0] == '+' && prevIsMorph == true) {
        	cerr << "POINT a";
            str.erase(str.begin());
            factor = fc.AddFactor(prevMorph + str, false);

          score -= prev_score;
      }
      else if (str[0] == '+' && prevIsMorph == false) {
          // Treat this as two separate words
        	cerr << "POINT b";
          str.erase(str.begin()); //Get rid of starting +
          factor = fc.AddFactor(str, false);
      }
      else if (str[0] != '+' && prevIsMorph == true) {
          // Treat this as two separate words
        	cerr << "POINT c";
      }
      else {
          //Yay! Easy ... just words
        	cerr << "POINT d";
      }
        
      if (str[str.length() - 1] == '+' && str.length() > 1) {
            str.erase(str.end() - 1);
            prevMorph = str;
            prevIsMorph = true;
            //TODO estimate score of the rest
      }
      else {
          prevMorph = "";
          prevIsMorph = false;
          //score -= KneserNey(stringContext); //TODO: Double check this is right
          // TODO: Subtract itermediate?
      }
    
      // If the current hypotheis is null, ignore it (just a +, start of this method, etc.)
      if (str.length() > 0) {
        context.push_back(factor);
        if (context.size() > m_order) {
    	  context.erase(context.begin());
        }
      }

      kn_score = KneserNey(context);
      score += kn_score;

      // If it is a morph, pop it off and keep it separate
      if (prevIsMorph) {
    	  context.pop_back();
          prev_score = kn_score;
      }
      else {
        prev_score = 0.0; // End of a word, don't need to subtract
      }


  }

  // finished scoring. set score
  accumulator->PlusEquals(this, score);

  // TODO: Subtract itermediate?
  if (context.size() >= m_order) {
	  context.erase(context.begin());
  }
  cerr << "prevMorph=" << prevMorph << endl;

  //std::vector<const Factor*>  context;
  //SetContext2(stringContext, context);

  assert(context.size() < m_order);
  return new MorphoLMState(context, prevMorph, prev_score);
}

FFState* MorphoLM::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  abort();
  return NULL;
}

float MorphoLM::KneserNey(std::vector<const Factor*> context) const
{
  assert(context.size() <= m_order);

  float backoff = 0.0;

  cerr << "CONTEXT:";
  //std::copy ( context.begin(), context.end(), std::ostream_iterator<string>(std::cerr,", ") );
  cerr << endl;

  Node<const Factor*, float>* result = root->getProb(context);

  float ret = -99999;
  if (result) {
    ret = result->getProb();
  }
  else if (context.size() > 1) {
	  std::vector<const Factor*> backOffContext(context.begin(), context.end() - 1);
	  result = root->getProb(backOffContext);
	  if (result) {
		  backoff = result->getBackOff();
	  }

	  context.erase(context.begin());

	ret = backoff + KneserNey(context);
  }
  else {
	  assert(context.size() == 1);
	  ret = m_oov;
  }
  
  cerr << "ret=" << ret << endl;
  return ret;
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
  else if (key == "marker") {
	  m_marker = value;
  }
  else if (key == "binary") {
	  m_binLM = Scan<bool>(value);
  }
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

void MorphoLM::SetContext(std::vector<std::string>  &context, const std::vector<const Factor*> &phrase) const
{
	for (size_t i = 0; i < phrase.size(); ++i) {
		context.push_back(phrase[i]->GetString().as_string());
	}

}

void MorphoLM::SetContext2(const std::vector<std::string>  &context, std::vector<const Factor*> &phrase) const
{
    FactorCollection &fc = FactorCollection::Instance();
	for (size_t i = 0; i < context.size(); ++i) {
		const Factor *factor = fc.AddFactor(context[i], false);
		phrase.push_back(factor);
	}

}

}


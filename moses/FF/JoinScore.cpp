#include <vector>
#include "JoinScore.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"

using namespace std;

namespace Moses
{
int JoinScoreState::Compare(const FFState& other) const
{
  const JoinScoreState &otherState = static_cast<const JoinScoreState&>(other);

  if (m_morphemes == otherState.m_morphemes)
    return 0;
  return (m_morphemes < otherState.m_morphemes) ? -1 : +1;
}

////////////////////////////////////////////////////////////////
JoinScore::JoinScore(const std::string &line)
  :StatefulFeatureFunction(4, line)
  ,m_scoreRealWords(true)
  ,m_scoreNumCompounds(true) 
  ,m_scoreInvalidJoins(true)
  ,m_scoreCompoundWord(true)
  ,m_maxMorphemeState(-1)
{
  ReadParameters();
}

void JoinScore::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedFutureScore) const
{}

void JoinScore::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{}

void JoinScore::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

FFState* JoinScore::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  const JoinScoreState *classState = static_cast<const JoinScoreState*>(prev_state);
  int prevJuncture = classState->GetJuncture();
  Phrase morphemes = classState->GetMorphemes();
  
  size_t numWord = 0;
  size_t numCompoundWord = 0;
  size_t numInvalidJoin = 0;
  float compoundWordScore = 0;

  const Phrase &tp = cur_hypo.GetCurrTargetPhrase();
  for (size_t pos = 0; pos < tp.GetSize(); ++pos) {
    const Word &word = tp.GetWord(pos);
    int currJuncture = GetJuncture(word);
    
    CalcScores(numWord, numCompoundWord, numInvalidJoin, compoundWordScore, 
              morphemes, word, 
              prevJuncture, currJuncture);
    prevJuncture = currJuncture;
  }

  // end of sentence
  if (cur_hypo.IsSourceCompleted()) {
    if (prevJuncture & 2) {
      ++numInvalidJoin;
      compoundWordScore += CalcMorphemeScore(morphemes);
      morphemes.Clear();
    }
  }

  // set scores
  vector<float> scores(m_numScoreComponents, 0);
  size_t ind = 0;
  if (m_scoreRealWords) {
    scores[ind++] = m_tuneable ? numWord : (numWord?-std::numeric_limits<float>::infinity():0);
  }
  if (m_scoreNumCompounds) {
    scores[ind++] = m_tuneable ? numCompoundWord : (numCompoundWord?-std::numeric_limits<float>::infinity():0);
  }
  if (m_scoreInvalidJoins) {
    scores[ind++] = m_tuneable ? numInvalidJoin : (numInvalidJoin?-std::numeric_limits<float>::infinity():0);    
  }
  if (m_scoreCompoundWord) {
    scores[ind++] = m_tuneable ? compoundWordScore : (compoundWordScore?-std::numeric_limits<float>::infinity():0);
  }
  UTIL_THROW_IF2(ind > m_numScoreComponents, "Vector element out-of-range:" << ind << ">" << m_numScoreComponents);
  
  accumulator->PlusEquals(this, scores);
  
  return new JoinScoreState(morphemes, prevJuncture);
}

void JoinScore::CalcScores(size_t &numWord, size_t&numCompoundWord, 
                          size_t &numInvalidJoin, float &compoundWordScore, 
                          Phrase &morphemes, const Word &morpheme,
                          int prevJuncture, int currJuncture) const
{
  if (prevJuncture < 0 || prevJuncture > 4 || currJuncture < 0 || currJuncture > 4) {
      UTIL_THROW2("Invalid juncture value: " << prevJuncture << " " << currJuncture);
  }
  
  switch (prevJuncture) {
    case 0:
      switch (currJuncture) {
        case 0:
          ++numWord;
          break;
        case 1:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;
          
          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 2:
          ++numWord;
          ++numCompoundWord;
          
          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          break;
        case 3:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;

          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          break;
        case 4:
          ++numWord;
          break;
      }
      break;
    case 1:
      switch (currJuncture) {
        case 0:
          ++numWord;
          break;
        case 1:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;
          
          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 2:
          ++numWord;
          ++numCompoundWord;
          
          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          break;
        case 3:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;

          assert(morphemes.GetSize() == 0);
          morphemes.AddWord(morpheme);
          break;
        case 4:
          ++numWord;
          break;
      }
      break;
    case 2:
      switch (currJuncture) {
        case 0:
          ++numInvalidJoin;
          ++numWord;
          
          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 1:
          assert(morphemes.GetSize());
          morphemes.AddWord(morpheme);
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 2:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          morphemes.AddWord(morpheme);
          break;
        case 3:
          assert(morphemes.GetSize() > 0);
          morphemes.AddWord(morpheme);
          break;
        case 4:
          break;
      }
      break;
    case 3:
      switch (currJuncture) {
        case 0:
          ++numInvalidJoin;
          ++numWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 1:
          assert(morphemes.GetSize());
          morphemes.AddWord(morpheme);
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 2:
          ++numInvalidJoin;
          ++numWord;
          ++numCompoundWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          morphemes.AddWord(morpheme);
          break;
        case 3:
          assert(morphemes.GetSize() > 0);
          morphemes.AddWord(morpheme);
          break;
        case 4:
          break;
      }
      break;
    case 4:
      switch (currJuncture) {
        case 0:
          ++numWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 1:
          assert(morphemes.GetSize());
          morphemes.AddWord(morpheme);
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          break;
        case 2:
          ++numWord;
          ++numCompoundWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          morphemes.AddWord(morpheme);
          break;
        case 3:
          break;
        case 4:
          ++numWord;

          assert(morphemes.GetSize());
          compoundWordScore += CalcMorphemeScore(morphemes);
          morphemes.Clear();
          morphemes.AddWord(morpheme);
          break;
      }
      break;
  }
}

FFState* JoinScore::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  return new JoinScoreState();
}

void JoinScore::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "score-real-words") {
    m_scoreRealWords = Scan<bool>(value);
  }
  else if (key == "score-num-compounds") {
    m_scoreNumCompounds = Scan<bool>(value);  
  }
  else if (key == "score-invalid-joins") {
    m_scoreInvalidJoins = Scan<bool>(value);      
  }
  else if (key == "score-compound-word") {
    m_scoreCompoundWord = Scan<bool>(value);  
  } 
  else if (key == "max-morpheme-state") {
    m_maxMorphemeState = Scan<int>(value);  
  } 
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

int JoinScore::GetJuncture(const Word &morpheme) const
{  
  if (morpheme.IsOOV()) {
      return 4;
  }
  
 int ret = 0;

 const Factor *factor = morpheme.GetFactor(0);
  StringPiece str = factor->GetString();
  if (str.size() > 1) {
    if (str.starts_with("+")) {
      ret += 1;
    }
    if (str.ends_with("+")) {
      ret += 2;
    }
  }
  
  return ret;
}

float JoinScore::CalcMorphemeScore(const Phrase &morphemes) const
{
  // use this function to score orthography of word
  return 0;
}

void JoinScore::AddMorphemeToState(Phrase &morphemes, const Word &morpheme)
{
  if (m_maxMorphemeState < 0) {
    // unlimited number of morph. Don't delete any prev morph
  }
  else if (m_maxMorphemeState == 0) {
    // stateless. Don't add any morphemes to state
    return;
  }
  else if (morphemes.GetSize() >= m_maxMorphemeState) {
    morphemes.RemoveWord(0);
  }
  
  morphemes.AddWord(morpheme);
}

}


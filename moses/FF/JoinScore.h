#pragma once

#include <boost/unordered_map.hpp>
#include <string>
#include "StatefulFeatureFunction.h"
#include "FFState.h"
#include "JoinScore/TrieSearch.h"

namespace Moses
{


class JoinScore : public StatefulFeatureFunction
{
////////////////////////////////////////////////////////////////
  class Node
  {
  public:
    bool isAWord;

    Node();
    Node *Insert(const std::string &tok);
    const Node *Find(const std::string &tok) const;

  protected:
    typedef boost::unordered_map<char, Node*> Children;
    Children m_children;

    Node *Insert(const std::string &tok, size_t pos);
    Node *GetOrCreateNode(char c);

    const Node *Find(const std::string &tok, size_t pos) const;
  };

////////////////////////////////////////////////////////////////

  class JoinScoreState : public FFState
  {
    //std::string m_concat;
    Phrase m_morphemes;
    size_t m_marker;
    //None  =  0,
    //Left  =  1,
    //Right =  2
    bool m_validCompound;

  public:
    JoinScoreState()
      :m_marker(0)
      ,m_validCompound(true)
    {}

    JoinScoreState(const JoinScoreState &copy)
      :m_morphemes(copy.m_morphemes)
      ,m_marker(copy.m_marker)
      ,m_validCompound(copy.m_validCompound)
    {}

    JoinScoreState(const Phrase &morphemes, size_t Marker, bool validCompound)
      :m_morphemes(morphemes)
      ,m_marker(Marker)
      ,m_validCompound(validCompound) {
    }

    int Compare(const FFState& other) const;
    FFState *Clone() const {
      return new JoinScoreState(*this);
    }

    const Phrase &GetMorphemes() const {
      return m_morphemes;
    }

    int GetMarker() const {
      return m_marker;
    }

    bool GetValidCompound() const {
      return m_validCompound;
    }
  };
////////////////////////////////////////////////////////////////

public:
  JoinScore(const std::string &line);

  virtual void Load();

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }
  virtual const FFState* EmptyHypothesisState(const InputType &input) const {
    return new JoinScoreState();
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

protected:
  bool m_scoreRealWords, m_scoreNumCompounds,
       m_scoreCompoundOOV,
       m_scoreCompoundWord, m_scorePartialCompound
       ;
  size_t m_scoreInvalidJoins;
  // 1 = a+|+a+ a|a+ = penalise dangling prefix
  // 2 = a|+a +a|+a+ = penalise dangling suffix
  bool m_scoreInvalidJoinsPrefix, m_scoreInvalidJoinsSuffix;

  int m_maxMorphemeState;
  float m_multiplier;
  std::string m_vocabPath;

  //Node m_vocabRoot;
  TrieSearch<bool> m_trieSearch;

  int GetMarker(const Word &word) const;
  void CalcScores(size_t &numWord, size_t&numCompoundWord,
                  size_t &numInvalidJoin, float &compoundWordScore, bool &validCompound,
                  Phrase &morphemes,
                  const Word *morpheme,
                  int prevMarker, int currMarker) const;
  float CalcMorphemeScore(const Phrase &morphemes, bool wholeWord, bool &validCompound) const;

  void AddMorphemeToState(Phrase &morphemes, const Word *morpheme) const;
  float CalcScore(float count) const;
  void SetInvalidJoins();

};


}


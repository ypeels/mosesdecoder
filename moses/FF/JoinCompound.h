#pragma once

#include <string>
#include <sstream>
#include "StatelessFeatureFunction.h"

namespace Moses
{
class Hypos;

class JoinCompound : public StatelessFeatureFunction
{
public:
  JoinCompound(const std::string &line);

  bool IsUseable(const FactorMask &mask) const {
    return true;
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

  void EvaluateWhenApplied(const Hypothesis& hypo,
                           ScoreComponentCollection* accumulator) const;
  void EvaluateWhenApplied(const ChartHypothesis &hypo,
                           ScoreComponentCollection* accumulator) const;


  void SetParameter(const std::string& key, const std::string& value);

  virtual void DoJoin(std::string &output);
  virtual void ChangeLattice(LatticeRescorerGraph &graph) const;

  void ChangeLattice(Hypos *hypos) const;
  void MergeHypos(const std::string &tpStrOrig, const Hypothesis *currHypo, Hypos *hypos) const;

  //None  =  0,
  //Left  =  1,
  //Right =  2
  size_t HasJuncture(const Word &word, std::string &stripped) const;
  size_t Desegment(std::string &tpStr, const Phrase &in) const;

};

}


#pragma once

#include <string>
#include <sstream>
#ifdef WITH_THREADS
#include <boost/thread/tss.hpp>
#else
#include <boost/scoped_ptr.hpp>
#include <ctime>
#endif

#include "StatelessFeatureFunction.h"

namespace Moses
{
class Hypos;
class LatticeRescorerNode;
class TranslationOption;

class DesegmentLattice : public StatelessFeatureFunction
{
public:
  DesegmentLattice(const std::string &line);

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

  virtual void CleanUpAfterSentenceProcessing(const InputType& source);

  void SetParameter(const std::string& key, const std::string& value);

  virtual void ChangeLattice(LatticeRescorerGraph &graph) const;

protected:
  typedef std::vector<TranslationOption*> CacheColl;

  struct HypoReplace
  {
	  Hypothesis *in;
	  std::vector<Hypothesis*> out;
	  std::vector<Hypos*> nodes;
  };
  typedef std::vector<HypoReplace> HypoReplaceColl;

  #ifdef WITH_THREADS
  //reader-writer lock
  mutable boost::thread_specific_ptr<CacheColl> m_cache;
  mutable boost::thread_specific_ptr<HypoReplaceColl> m_hypoReplace;
#else
  mutable boost::scoped_ptr<CacheColl> m_cache;
  mutable boost::scoped_ptr<HypoReplaceColl> m_hypoReplace;
#endif

  CacheColl &GetCache() const;
  HypoReplaceColl &GetHypoReplaceCache() const;

  void ChangeLattice(Hypos *hypos) const;
  void MergeHypos(const std::string &tpStrOrig, const HypoReplace &hyposReplacedOrig, LatticeRescorerNode &node) const;
  void CreateHypo(const std::string str, HypoReplace hyposReplaced) const;

  //None  =  0,
  //Left  =  1,
  //Right =  2
  size_t HasJuncture(const Word &word, std::string &stripped) const;
  size_t Desegment(std::string &tpStr, const Phrase &in) const;

  void ResetPrevHypo(LatticeRescorerNode &node, const Hypothesis *oldPrevHypo, const Hypothesis *newPrevHypo) const;

};

}


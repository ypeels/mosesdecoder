#pragma once

#ifdef WITH_THREADS
#include <boost/thread/tss.hpp>
#else
#include <boost/scoped_ptr.hpp>
#include <time.h>
#endif

#include <string>
#include "StatelessFeatureFunction.h"

namespace Moses
{
class StackVec;
class ChartTranslationOption;


class RuleDiscr : public StatelessFeatureFunction
{
public:
  RuleDiscr(const std::string &line);

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }

  //! Called before search and collecting of translation options
  virtual void InitializeForInput(InputType const& source);

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
  void EvaluateWhenApplied(const Hypothesis& hypo,
                           ScoreComponentCollection* accumulator) const;
  void EvaluateWhenApplied(const ChartHypothesis &hypo,
                           ScoreComponentCollection* accumulator) const;
  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const
  {}
  void EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList, const ChartCellCollection &hypoStackColl) const;

  void SetParameter(const std::string& key, const std::string& value);

protected:
  bool m_insideScores;
  int m_whatScores; // 0 (default) = pef only, 1 = stateless, 2 = all
  const FeatureFunction &m_pt;
  
  // cache
  size_t m_maxCacheSize; // 0 = no caching

  typedef boost::unordered_map<size_t, std::pair<float, clock_t> > MaxProbCache;
#ifdef WITH_THREADS
  //reader-writer lock
  mutable boost::thread_specific_ptr<MaxProbCache> m_maxProbCache;
#else
  mutable boost::scoped_ptr<MaxProbCache> m_maxProbCache;
#endif

  MaxProbCache &GetMaxProbCache() const;
  void ReduceCache() const;

  float GetScore(const ChartTranslationOption &transOpt) const;
  float GetScore(const ChartHypothesis &hypo) const;

  float GetBestHypoScores(const ChartCellCollection &hypoStackColl
                        , const StackVec &stackVec) const;

};

}


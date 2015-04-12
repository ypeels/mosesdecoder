#include <vector>
#include <queue>
#include <boost/foreach.hpp>

#include "RuleDiscr.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/ChartTranslationOptionList.h"
#include "moses/ChartTranslationOptions.h"
#include "moses/PP/CountsPhraseProperty.h"
#include "moses/Timer.h"
#include "moses/StaticData.h"
#include "moses/ChartCellLabel.h"
#include "moses/ChartCell.h"
#include "moses/ChartCellCollection.h"

using namespace std;

namespace Moses
{
RuleDiscr::RuleDiscr(const std::string &line)
  :StatelessFeatureFunction(1, line)
  ,m_maxCacheSize(DEFAULT_MAX_TRANS_OPT_CACHE_SIZE)
  ,m_insideScores(false)
  ,m_whatScores(0)
  ,m_pt(FeatureFunction::FindFeatureFunction("TranslationModel0"))
{
  m_requireSortingAfterSourceContext = true;
  ReadParameters();
}

void RuleDiscr::InitializeForInput(InputType const& source)
{
  ReduceCache();
}


void RuleDiscr::EvaluateInIsolation(const Phrase &source
                                      , const TargetPhrase &targetPhrase
                                      , ScoreComponentCollection &scoreBreakdown
                                      , ScoreComponentCollection &estimatedFutureScore) const
{
  targetPhrase.SetRuleSource(source);
}

void RuleDiscr::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{
}

void RuleDiscr::EvaluateWhenApplied(const Hypothesis& hypo,
                                      ScoreComponentCollection* accumulator) const
{}

void RuleDiscr::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                      ScoreComponentCollection* accumulator) const
{}

void RuleDiscr::EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList, const ChartCellCollection &hypoStackColl) const
{	
  // find max p(e|f)
  float maxPEF = - std::numeric_limits<float>::infinity();
  //cerr << "ChartTranslationOptionList:" << endl;
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    //cerr << "ChartTranslationOptions " << i << "=" << transOpts.GetSize() << endl;
    
    if (transOpts.GetSize() == 0) continue;
        
    const TargetPhrase &tp = transOpts.Get(0).GetPhrase();
    const Phrase *sp = tp.GetRuleSource();
    assert(sp);
    size_t hash = hash_value(*sp);
    
    MaxProbCache &cache = GetMaxProbCache();
    MaxProbCache::iterator cacheIter = cache.find(hash);
    
    float maxPEFTransOpts = - std::numeric_limits<float>::infinity();
    if (cacheIter == cache.end()) {
      // not in cache. find max pef
      float bestHyposScore = 0;
      if (m_insideScores) {
        bestHyposScore = GetBestHypoScores(hypoStackColl, transOpts.GetStackVec());
      }
  
      for (size_t j = 0; j < transOpts.GetSize(); ++j) {
        const ChartTranslationOption &transOpt = transOpts.Get(j);
        //cerr << "   " << transOpt << endl;

        float pef = bestHyposScore + GetScore(transOpt);
        
        if (maxPEFTransOpts < pef) {
          maxPEFTransOpts = pef;
        }
      }
    
      // update cache
      std::pair<float, clock_t> value(maxPEFTransOpts, clock());
      cache[hash] = value;
    }
    else {
      // in cache. use it
      std::pair<float, clock_t> &value = cacheIter->second;
      value.second = clock();

      maxPEFTransOpts = value.first;
    }
    
    if (maxPEF < maxPEFTransOpts) {
      maxPEF = maxPEFTransOpts;
    }

  }

  // set scores
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    //cerr << "ChartTranslationOptions " << i << "=" << transOpts.GetSize() << endl;

    if (transOpts.GetSize() == 0) continue;
    
    float bestHyposScore = 0;
    if (m_insideScores) {
      bestHyposScore = GetBestHypoScores(hypoStackColl, transOpts.GetStackVec());
    }

    for (size_t j = 0; j < transOpts.GetSize(); ++j) {
    	ChartTranslationOption &transOpt = transOpts.Get(j);
    	//cerr << "   " << transOpt << endl;

      float pef = bestHyposScore + GetScore(transOpt);

      float diff = maxPEF - pef;

      transOpt.GetScores().PlusEquals(this, diff);
    }
  }
}

float RuleDiscr::GetScore(const ChartTranslationOption &transOpt) const
{
  float ret;
  
  switch (m_whatScores) {
    case 0:
    {
      std::vector<float> scores = transOpt.GetScores().GetScoresForProducer(&m_pt);
      ret = scores[2];
      break;
    }
    case 1:
    {
      ScoreComponentCollection statelessScores;
      
      BOOST_FOREACH(const StatelessFeatureFunction *ff, StatelessFeatureFunction::GetStatelessFeatureFunctions()) {
        statelessScores.PlusEquals(ff, transOpt.GetScores());
      }
      
      ret = statelessScores.GetWeightedScore();
      break;
    }
    case 2:
    {
      ret = transOpt.GetScores().GetWeightedScore();
      break;
    }
    default:
        UTIL_THROW2("Unknown what-score: " << m_whatScores);
  }
  
  return ret;
}

float RuleDiscr::GetScore(const ChartHypothesis &hypo) const
{
  float ret;
  
  switch (m_whatScores) {
    case 0:
    {
      std::vector<float> scores = hypo.GetScoreBreakdown().GetScoresForProducer(&m_pt);
      ret = scores[2];
      break;
    }
    case 1:
    {
      ScoreComponentCollection statelessScores;
      
      BOOST_FOREACH(const StatelessFeatureFunction *ff, StatelessFeatureFunction::GetStatelessFeatureFunctions()) {
        statelessScores.PlusEquals(ff, hypo.GetScoreBreakdown());
      }
      
      ret = statelessScores.GetWeightedScore();
      break;
    }
    case 2:
    {
      ret = hypo.GetScoreBreakdown().GetWeightedScore();
      break;
    }
    default:
        UTIL_THROW2("Unknown what-score: " << m_whatScores);
  }
  
  return ret;
}

float RuleDiscr::GetBestHypoScores(const ChartCellCollection &hypoStackColl
                                  , const StackVec &stackVec) const
{
  float ret = 0;
  
  size_t numNT = stackVec.size();
  for (size_t i = 0; i < numNT; ++i) {
    const ChartCellLabel &label = *stackVec[i];
    const WordsRange &range = label.GetCoverage();
    const ChartCell &cell = hypoStackColl.Get(range);
    const ChartHypothesis *hypo = cell.GetBestHypothesis();
    
    UTIL_THROW_IF2(hypo == NULL, "No hypos at range " << range);

    ret += GetScore(*hypo);
  }
  
  return ret;
}

void RuleDiscr::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "inside-score") {
    m_insideScores = Scan<bool>(value);
  }
  else if (key == "what-scores") {
    m_whatScores = Scan<int>(value);
  }
  else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

RuleDiscr::MaxProbCache &RuleDiscr::GetMaxProbCache() const
{
  MaxProbCache *cache;
  cache = m_maxProbCache.get();
  if (cache == NULL) {
    cache = new MaxProbCache;
    m_maxProbCache.reset(cache);
  }
  assert(cache);
  return *cache;

}

// reduce presistent cache by half of maximum size
void RuleDiscr::ReduceCache() const
{
  Timer reduceCacheTime;
  reduceCacheTime.start();
  MaxProbCache &cache = GetMaxProbCache();
  if (cache.size() <= m_maxCacheSize) return; // not full

  // find cutoff for last used time
  priority_queue< clock_t > lastUsedTimes;
  MaxProbCache::iterator iter;
  iter = cache.begin();
  while( iter != cache.end() ) {
    lastUsedTimes.push( iter->second.second );
    iter++;
  }
  for( size_t i=0; i < lastUsedTimes.size()-m_maxCacheSize/2; i++ )
    lastUsedTimes.pop();
  clock_t cutoffLastUsedTime = lastUsedTimes.top();

  // remove all old entries
  iter = cache.begin();
  while( iter != cache.end() ) {
    if (iter->second.second < cutoffLastUsedTime) {
      MaxProbCache::iterator iterRemove = iter++;
      cache.erase(iterRemove);
    } else iter++;
  }
  VERBOSE(2,"Reduced persistent translation option cache in " << reduceCacheTime << " seconds." << std::endl);
}
}


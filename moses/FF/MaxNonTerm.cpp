#include <vector>
#include "MaxNonTerm.h"
#include "moses/ScoreComponentCollection.h"
#include "moses/TargetPhrase.h"
#include "moses/ChartTranslationOptionList.h"
#include "moses/ChartTranslationOptions.h"
#include "moses/PP/CountsPhraseProperty.h"
#include "moses/TranslationModel/PhraseDictionary.h"

using namespace std;

namespace Moses
{
MaxNonTerm::MaxNonTerm(const std::string &line)
  :StatelessFeatureFunction(0, line)
  ,m_maxNonTerm(9999)
  ,m_pt(NULL)
{
  m_tuneable = false;
  ReadParameters();
}

std::vector<float> MaxNonTerm::DefaultWeights() const
{
  UTIL_THROW_IF2(m_numScoreComponents != 0, "No scores here");
  vector<float> ret(0);
  return ret;
}

void MaxNonTerm::EvaluateInIsolation(const Phrase &source
                                     , const TargetPhrase &targetPhrase
                                     , ScoreComponentCollection &scoreBreakdown
                                     , ScoreComponentCollection &estimatedFutureScore) const
{
}

void MaxNonTerm::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedFutureScore) const
{
}

void MaxNonTerm::EvaluateWhenApplied(const Hypothesis& hypo,
                                     ScoreComponentCollection* accumulator) const
{}

void MaxNonTerm::EvaluateWhenApplied(const ChartHypothesis &hypo,
                                     ScoreComponentCollection* accumulator) const
{}

void MaxNonTerm::EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList) const
{
  if (transOptList.GetSize() == 0) {
    return;
  }

  // transopts to be deleted
  typedef set<ChartTranslationOptions*> Coll;
  Coll transOptsToDelete;

  //cerr << "ChartTranslationOptionList:" << endl;
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    if (transOpts.GetSize() == 0) {
      continue;
    }

    // only prune for a particular pt
    if (m_pt) {
      const ChartTranslationOption &transOpt = transOpts.Get(0);
      const TargetPhrase &tp = transOpt.GetPhrase();
      const PhraseDictionary *tpPt = tp.GetContainer();
      if (tpPt != m_pt) {
        return;
      }
    }

    const ChartTranslationOption &transOpt = transOpts.Get(0);
    const TargetPhrase &tp = transOpt.GetPhrase();
    size_t numNT = tp.GetNumNonTerminals();

    if (numNT > m_maxNonTerm) {
      transOptsToDelete.insert(&transOpts);
    }
  }

  // delete
  Coll::iterator iter;
  for (iter = transOptsToDelete.begin(); iter != transOptsToDelete.end(); ++iter) {
    ChartTranslationOptions *transOpts = *iter;
    transOpts->Clear();
  }
}

void MaxNonTerm::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "max-non-term") {
    m_maxNonTerm = Scan<size_t>(value);
  } else if (key == "phrase-table") {
    FeatureFunction &ff = FindFeatureFunction(value, 0);
    m_pt = dynamic_cast<const PhraseDictionary*>(&ff);
    UTIL_THROW_IF2(m_pt == NULL, "Not a phrase-table: " << value);
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

}


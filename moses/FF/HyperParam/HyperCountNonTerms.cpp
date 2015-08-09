#include <cassert>
#include "HyperCountNonTerms.h"
#include "moses/Util.h"
#include "moses/TargetPhrase.h"
#include "moses/ChartTranslationOptionList.h"
#include "moses/ChartTranslationOptions.h"
#include "moses/StaticData.h"
#include "moses/TranslationModel/PhraseDictionary.h"

using namespace std;

namespace Moses
{
HyperCountNonTerms::HyperCountNonTerms(const std::string &line)
  :StatelessFeatureFunction(2, line)
  ,m_pt(NULL)
{
  ReadParameters();
}

void HyperCountNonTerms::EvaluateWithAllTransOpts(ChartTranslationOptionList &transOptList) const
{

  if (transOptList.GetSize() == 0) {
    return;
  }

  // only prune for a particular pt
  if (m_pt) {
    ChartTranslationOptions &transOpts = transOptList.Get(0);
    const ChartTranslationOption &transOpt = transOpts.Get(0);
    const TargetPhrase &tp = transOpt.GetPhrase();
    const PhraseDictionary *tpPt = tp.GetContainer();
    if (tpPt != m_pt) {
      return;
    }
  }

  // transopts to be deleted
  typedef set<ChartTranslationOptions*> Coll;
  Coll transOptsToDelete;

  std::vector<float> weights = StaticData::Instance().GetWeights(this);
  assert(weights.size() == 2);

  // collect counts
  //cerr << "ChartTranslationOptionList:" << endl;
  for (size_t i = 0; i < transOptList.GetSize(); ++i) {
    ChartTranslationOptions &transOpts = transOptList.Get(i);
    const WordsRange &range = transOpts.GetSourceWordsRange();
    if (range.GetStartPos() == 0) {
      // glue rule. ignore
      continue;
    }

    //cerr << "ChartTranslationOptions " << i << "=" << transOpts.GetSize() << endl;

    /*
    for (size_t j = 0; j < transOpts.GetSize(); ++j) {
    	const ChartTranslationOption &transOpt = transOpts.Get(j);
    	cerr << "   " << transOpt << endl;
    }
    */

    UTIL_THROW_IF2(transOpts.GetSize() == 0, "transOpts can't be empty");

    assert(transOpts.GetSize());
    const ChartTranslationOption &transOpt = transOpts.Get(0);
    const TargetPhrase &tp = transOpt.GetPhrase();
    size_t numNT = tp.GetNumNonTerminals();

    if (numNT == 0) {
      // no NT. Keep the rules
      continue;
    }
    float weight = weights[numNT - 1];
    if (weight > 0) {
      // delete rules
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

void HyperCountNonTerms::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "phrase-table") {
    FeatureFunction &ff = FindFeatureFunction(value, 0);
    m_pt = dynamic_cast<const PhraseDictionary*>(&ff);
    UTIL_THROW_IF2(m_pt == NULL, "Not a phrase-table: " << value);
  } else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}


}

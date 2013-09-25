
#pragma once

#include "PhraseDictionary.h"

namespace Moses
{
class ChartParser;
class ChartCellCollectionBase;
class ChartRuleLookupManager;

class SkeletonPT : public PhraseDictionary
{
  friend std::ostream& operator<<(std::ostream&, const SkeletonPT&);

public:
  SkeletonPT(const std::string &line);

  virtual void CleanUpAfterSentenceProcessing(const InputType& source);

  // for phrase-based model
  void GetTargetPhraseCollectionBatch(const InputPathList &phraseDictionaryQueue) const;

  // for syntax/hiero model (CKY+ decoding)
  ChartRuleLookupManager* CreateRuleLookupManager(const ChartParser&, const ChartCellCollectionBase&);

  TO_STRING();

protected:
  mutable std::list<TargetPhraseCollection*> m_allTPColl;

  TargetPhrase *CreateTargetPhrase(const Phrase &sourcePhrase) const;
};

}  // namespace Moses

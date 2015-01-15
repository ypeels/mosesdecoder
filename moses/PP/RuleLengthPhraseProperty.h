
#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include "moses/PP/PhraseProperty.h"

namespace Moses
{

class RuleLengthPhraseProperty : public PhraseProperty
{
public:
  RuleLengthPhraseProperty();

  void ProcessValue(const std::string &value);

  float GetProb(size_t sourceWidth, float smoothing) const;
protected:
  // fractional counts
  typedef std::map<size_t, float> Map;
  Map m_counts;
  float m_total;

  void CalcTotal();
};

} // namespace Moses


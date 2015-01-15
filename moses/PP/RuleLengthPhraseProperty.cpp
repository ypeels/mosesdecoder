#include "RuleLengthPhraseProperty.h"
#include "moses/Util.h"
#include "util/exception.hh"

using namespace std;

namespace Moses
{
RuleLengthPhraseProperty::RuleLengthPhraseProperty()
{
}

void RuleLengthPhraseProperty::ProcessValue(const std::string &value)
{
  vector<string> toks;
  Tokenize(toks, value);

  for (size_t i = 0; i < toks.size(); i += 2) {
    size_t length = Scan<size_t>(toks[i]);
    float count = Scan<float>(toks[i+1]);
    m_counts[length] = count;
  }

  // totals
  CalcTotal();
}

void RuleLengthPhraseProperty::CalcTotal()
{
  m_total = 0;
  Map::const_iterator iter;
  for (iter = m_counts.begin(); iter != m_counts.end(); ++iter) {
    m_total += iter->second;
  }
}

float RuleLengthPhraseProperty::GetProb(size_t sourceWidth, float smoothing) const
{
  float count;

  Map::const_iterator iter = m_counts.find(sourceWidth);
  if (iter == m_counts.end()) {
    count = 0;
  } else {
    count = iter->second;
  }
  count += smoothing;

  float total = m_total + smoothing * (float) m_counts.size();
  float ret = count / total;
  return ret;
}


}

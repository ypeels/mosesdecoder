#pragma once

#include <string>
#include "Classifier.h"
#include "moses/TypeDef.h"
#include "moses/Util.h"
#include "moses/FF/StatelessFeatureFunction.h"

namespace Moses
{

class VWFeatureBase : public StatelessFeatureFunction
{
  public:
    VWFeatureBase(const std::string &line)
      :StatelessFeatureFunction(0, line)
    {
      ReadParameters();

      if(m_usedBy.empty())
        m_usedBy.push_back("VW0");

      for(std::vector<std::string>::const_iterator it = m_usedBy.begin();
          it != m_usedBy.end(); it++)
        s_features[*it].push_back(this);
    }

    bool IsUseable(const FactorMask &mask) const {
      return true;
    }

    // Official hooks do nothing
    void EvaluateInIsolation(const Phrase &source
                  , const TargetPhrase &targetPhrase
                  , ScoreComponentCollection &scoreBreakdown
                  , ScoreComponentCollection &estimatedFutureScore) const {}
    void EvaluateWithSourceContext(const InputType &input
                  , const InputPath &inputPath
                  , const TargetPhrase &targetPhrase
                  , const StackVec *stackVec
                  , ScoreComponentCollection &scoreBreakdown
                  , ScoreComponentCollection *estimatedFutureScore = NULL) const {}

    void EvaluateTranslationOptionListWithSourceContext(const InputType &input
                  , const TranslationOptionList &translationOptionList) const {}

    void EvaluateWhenApplied(const Hypothesis& hypo,
                  ScoreComponentCollection* accumulator) const {}
    void EvaluateWhenApplied(const ChartHypothesis &hypo,
                       ScoreComponentCollection* accumulator) const {}

    void SetParameter(const std::string& key, const std::string& value)
    {
      if (key == "used-by") {
        ParseUsedBy(value);
      } else if (key == "source-factors") {
        ParseFactorDefinition(value, m_sourceFactors);
      } else if (key == "target-factors") {
        ParseFactorDefinition(value, m_targetFactors);
      } else {
        StatelessFeatureFunction::SetParameter(key, value);
      }
    }

    virtual void operator()(const InputType &input
                  , const InputPath &inputPath
                  , const TargetPhrase &targetPhrase
                  , Discriminative::Classifier *classifier) const = 0;

    static const std::vector<VWFeatureBase*>& GetFeatures(std::string name = "VW0") {
      UTIL_THROW_IF2(s_features.count(name) == 0, "No features registered for parent classifier: " + name);
      return s_features[name];
    }

  protected:
    std::vector<FactorType> m_sourceFactors, m_targetFactors;

  private:
    void ParseFactorDefinition(const std::string &list, /* out */ std::vector<FactorType> &out)
    {
      std::vector<std::string> split = Tokenize(list, ",");
      Scan<int>(out, split);
    }

    void ParseUsedBy(const std::string &usedBy) {
      Tokenize(m_usedBy, usedBy, ",");
    }

    std::vector<std::string> m_usedBy;
    static std::map<std::string, std::vector<VWFeatureBase*> > s_features;
};

}

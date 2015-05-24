#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <list>

typedef std::vector<std::string> Word;
typedef std::vector<Word> Phrase;

struct Range
{
  Range(int start,int end)
  :range(start, end)
  {}

  std::pair<int,int> range;
};

struct RangeWithLabel : public Range
{
  RangeWithLabel(int start,int end)
  :Range(start, end)
  {}

  RangeWithLabel(int start,int end, const std::string &l)
  :Range(start, end)
  ,label(l)
  {}

  std::string label;
};

typedef std::list<RangeWithLabel> Ranges;

bool IsA(const Phrase &source, int pos, int offset, int factor, const std::string &str);
void OutputWithLabels(const Phrase &source, const Ranges ranges, std::ostream &out);



#pragma once

#include <vector>
#include <map>
#include <string>
#include "moses/InputFileStream.h"

typedef std::map<std::string, int> Counts;

void Compare(Moses::InputFileStream &refStrme, Moses::InputFileStream &in1Strme, Moses::InputFileStream &in2Strme);
void Compare(const std::vector<std::string> &toksRef, const std::vector<std::string> &toksIn1, const std::vector<std::string> &toksIn2);
void AddToCounts(Counts &counts, const std::vector<std::string> &toks);
bool SubtractFromCounts(Counts &counts, const std::string &tok);


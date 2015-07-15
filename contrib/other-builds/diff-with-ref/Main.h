#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <string>

typedef std::map<std::string, int> Counts;

void Compare(std::ifstream &refStrme, std::ifstream &in1Strme, std::ifstream &in2Strme);
void Compare(const std::vector<std::string> &toksRef, const std::vector<std::string> &toksIn1, const std::vector<std::string> &toksIn2, int lineNum);
void AddToCounts(Counts &counts, const std::vector<std::string> &toks);
bool SubtractFromCounts(Counts &counts, const std::string &tok);


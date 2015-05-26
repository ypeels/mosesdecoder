#pragma once

#include <string>
#include <vector>
#include <map>
#include "LCS.h"

class Parameter;

// speeded up version of above
inline void Tokenize(std::vector<std::string> &output
                     , const std::string& str
                     , const std::string& delimiters = " \t")
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    output.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void ProcessLineChar(std::vector<Point> &alignments, const Parameter &params, const std::vector<std::string> &toksX, const std::vector<std::string> &toksY);
void ProcessLineLCS(std::vector<Point> &alignments, const Parameter &params, const std::vector<std::string> &toksX, const std::vector<std::string> &toksY);
void CreateMismatches(std::vector<Point> &mismatches, int startMismatchX, int endMismatchX, int startMismatchY, int endMismatchY);
void CreateCrossProduct(std::vector<Point> &alignments, const std::vector<int> &indsX, const std::vector<int> &indsY);


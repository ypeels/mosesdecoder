#pragma once
#include <string>
#include <vector>

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

inline std::vector<std::string> TokenizeMultiCharSeparator(
  const std::string& str,
  const std::string& separator)
{
  std::vector<std::string> tokens;

  size_t pos = 0;
  // Find first "non-delimiter".
  std::string::size_type nextPos     = str.find(separator, pos);

  while (nextPos != std::string::npos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(pos, nextPos - pos));
    // Skip delimiters.  Note the "not_of"
    pos = nextPos + separator.size();
    // Find next "non-delimiter"
    nextPos	= str.find(separator, pos);
  }
  tokens.push_back(str.substr(pos, nextPos - pos));

  return tokens;
}


#include <iostream>
#include <fstream>
#include <cassert>
#include "Main.h"
#include "moses/Util.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  string refPath = argv[1];
  string in1Path = argv[2];
  string in2Path = argv[3];

  Moses::InputFileStream refStrme(refPath);
  Moses::InputFileStream in1Strme(in1Path);
  Moses::InputFileStream in2Strme(in2Path);

  Compare(refStrme, in1Strme, in2Strme);
  
	cerr << "finished" << endl;
	return 0;
}

void Compare(Moses::InputFileStream &refStrme, Moses::InputFileStream &in1Strme, Moses::InputFileStream &in2Strme)
{
  
  string refLine, in1Line, in2Line;
  while (getline(refStrme, refLine)) {
    getline(in1Strme, in1Line);
    getline(in2Strme, in2Line);
    
    vector<string> toksRef, toksIn1, toksIn2;
    Moses::Tokenize(toksRef, refLine);
    Moses::Tokenize(toksIn1, in1Line);
    Moses::Tokenize(toksIn2, in2Line);
    Compare(toksRef, toksIn1, toksIn2);
    
  }
}

void Compare(const std::vector<std::string> &toksRef, const std::vector<std::string> &toksIn1, const std::vector<std::string> &toksIn2)
{
  Counts counts1, counts2;
  AddToCounts(counts1, toksIn1);
  AddToCounts(counts2, toksIn2);

  for (auto &tok : toksRef ) {
    bool exist1 = SubtractFromCounts(counts1, tok);
    bool exist2 = SubtractFromCounts(counts2, tok);
  }   
}

void AddToCounts(Counts &counts, const std::vector<std::string> &toks)
{
  for (auto &tok : toks ) {
    Counts::iterator iter = counts.find(tok);
    if (iter == counts.end()) {
      counts[tok] = 1;
    }
    else {
      int &count = iter->second;
      ++count;
    }
  }
}

bool SubtractFromCounts(Counts &counts, const std::string &tok)
{
  Counts::iterator iter = counts.find(tok);
  if (iter == counts.end()) {
    return false;
  }
  else {
    int &count = iter->second;
    if (count == 0) {
      return false;
    }
    else {
      --count;
      return true;
    }
  }
}
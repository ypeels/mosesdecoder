#include <iostream>
#include <fstream>
#include <cassert>
#include "Main.h"
#include "moses/Util.h"
#include "moses/FF/JoinScore/TrieSearch.h"

using namespace std;

Moses::TrieSearch<bool> *m_trieSearch = NULL;
vector<int> analysis(4, 0);

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  string refPath = argv[1];
  string in1Path = argv[2];
  string in2Path = argv[3];

	if (argc >= 5) {
	  string m_vocabPath = argv[4];
	  m_trieSearch = new Moses::TrieSearch<bool>;
		m_trieSearch->Create(m_vocabPath);
	}
	
	/*
  Moses::InputFileStream refStrme(refPath);
  Moses::InputFileStream in1Strme(in1Path);
  Moses::InputFileStream in2Strme(in2Path);
	*/
	ifstream refStrme, in1Strme, in2Strme;
	refStrme.open(refPath.c_str());
	in1Strme.open(in1Path.c_str());
	in2Strme.open(in2Path.c_str());
	
  Compare(refStrme, in1Strme, in2Strme);
  
  cout << "EXISTS in: neither \t 1st, not 2nd \t 2nd, not 1st \t both \n";
  cout << "\t ";
  for (size_t i = 0; i < analysis.size(); ++i) {
		cout << analysis[i] << "\t ";
	}
	cout << endl;
	
	cerr << "finished" << endl;
	return 0;
}

void Compare(std::ifstream &refStrme, std::ifstream &in1Strme, std::ifstream &in2Strme)
{
  int lineNum = 1;
  string refLine, in1Line, in2Line;
  while (getline(refStrme, refLine)) {
    getline(in1Strme, in1Line);
    getline(in2Strme, in2Line);
    
    vector<string> toksRef, toksIn1, toksIn2;
    Moses::Tokenize(toksRef, refLine);
    Moses::Tokenize(toksIn1, in1Line);
    Moses::Tokenize(toksIn2, in2Line);
    Compare(toksRef, toksIn1, toksIn2, lineNum);
    
    ++lineNum;
  }
}

void Compare(const std::vector<std::string> &toksRef, const std::vector<std::string> &toksIn1, const std::vector<std::string> &toksIn2, int lineNum)
{
  Counts counts1, counts2;
  AddToCounts(counts1, toksIn1);
  AddToCounts(counts2, toksIn2);

  for (size_t i = 0; i < toksRef.size(); ++i) {
  	const string &tok = toksRef[i];
    bool exist1 = SubtractFromCounts(counts1, tok);
    bool exist2 = SubtractFromCounts(counts2, tok);

		if (exist1) {
			if (exist2) {
				++analysis[3];
			}
			else {
				++analysis[1];
			}		
		}
		else {
			if (exist2) {
				++analysis[2];
				cout << lineNum << "=" << tok;

				if (m_trieSearch) {
					bool found, isAWord;
					found = m_trieSearch->Find(isAWord, tok);
					if (found && isAWord) {
						cout << " FOUND";
					}
					else {
						cout << " NOT";
					}
				}			
				cout << endl;
			}
			else {
				++analysis[0];
			}
		}    
  }   
}

void AddToCounts(Counts &counts, const std::vector<std::string> &toks)
{
  for (size_t i = 0; i < toks.size(); ++i) {
  	const string &tok = toks[i];
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
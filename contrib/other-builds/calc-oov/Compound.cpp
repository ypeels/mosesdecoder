#include <cassert>
#include <vector>
#include <iostream>
#include "Compound.h"
#include "Main.h"

using namespace std;

void Compound::CreateVocab(std::ifstream &corpusStrme)
{
  string line;
	vector<string> toks;
	while (getline(corpusStrme, line)) {
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			m_root.Insert(tok);
		}
		
		toks.clear();
	}

}

void Compound::CalcOOV(std::ifstream &testStrme, std::ofstream &outStream) const
{
  size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> oovTypes, foundTypes;
	
	string line;
	while (getline(testStrme, line)) {
    vector<string> toks;
		 Tokenize(toks, line);
		 for (size_t i = 0; i < toks.size(); ++i) {
      const string &tok = toks[i];
			
			bool found = Decode(tok);

  		if ( !found ) {
       outStream << tok << endl;
       ++oovToks;
    		
    		oovTypes.insert(tok);
    	}
    	else {
    		foundTypes.insert(tok);
    	}
    	
    	++totalToks;
		}	
			
	}
	
	float tokRatio = (float) oovToks / (float) totalToks;
	
	float totalTypes = oovTypes.size() + foundTypes.size();
	float typeRatio = (float) oovTypes.size() / totalTypes;
	
	outStream << "totalToks=" << totalToks 
			<< " oovToks=" << oovToks << "(" << tokRatio << ")"
			<< " totalTypes=" << totalTypes
			<< " oovTypes=" << oovTypes.size() << "(" << typeRatio << ")" << endl;
}

bool Compound::Decode(const std::string &tok) const
{
  std::unordered_set<size_t> stack;
  stack.insert(0);
  
  while (!stack.empty()) {
    std::unordered_set<size_t>::iterator iter = stack.begin();
    size_t startPos = *iter;
    stack.erase(iter);
    
    bool ret = Decode(stack, tok, startPos);
    if (ret) {
      return true;
    }
  }
  
  // still can't find a completed word
  return false;
  
}

bool Compound::Decode(std::unordered_set<size_t> &stack, const std::string &tok, size_t startPos) const
{
  const Node *node = &m_root;
  for (size_t currPos = startPos; currPos < tok.size(); ++currPos) {
    char c = tok[currPos];
    node = node->Find(c);
    
    if (node) {
      if (node->isAWord) {
        stack.insert(currPos + 1);
      }
    }
    else {
      // couldn't get to the end
      return false;
    }
  }
  
  // got to the end but not is still not a word
  return node->isAWord;
}


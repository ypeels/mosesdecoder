#include <cassert>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Compound.h"
#include "Main.h"
#include "Parameter.h"

using namespace std;

Compound::Compound()
{
  cerr << "Compound\n";  
}

void Compound::CreateVocab(std::ifstream &corpusStrme, const Parameter &params)
{
  const string &j = params.juncture;
  
  string line;
	vector<string> toks;
	while (getline(corpusStrme, line)) {
    if (params.lowercase) {
        boost::algorithm::to_lower(line);
    }
		//cerr << line << endl;

		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
  		/*cerr << "tok=" << tok << " j=" << j << " " 
          << tok.size() << " " << j.size() << " "
          << tok.rfind(j) << endl;
      */
      
      if (params.juncture.empty()) {
        // not using juncture. endjuncture == isWord
        //cerr << "insert a " << tok << endl;
        m_root.Insert(tok, true, true);
      }
      else if (tok.size() > j.size() 
          && tok.rfind(j) == (tok.size() - j.size())) {
        // has end juncture
        string tempTok = tok.substr(0, tok.size() - j.size());
        //cerr << "insert b " << tempTok << endl;
        m_root.Insert(tempTok, true, false);        
      }
      else {
        // using juncture but this word doesn't have endJuncture
        //cerr << "insert c " << tok << endl;
        m_root.Insert(tok, false, true);                
      }
		}
		
		toks.clear();
	}

  // juncture root
  if (params.juncture.empty()) {
    m_rootJuncture = &m_root;
  }
  else {
    m_rootJuncture = m_root.Insert(params.juncture, false, false);
  }
}

void Compound::CalcOOV(std::ifstream &testStrme, const Parameter &params) const
{
  size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> oovTypes, foundTypes;
	
	string line;
	while (getline(testStrme, line)) {
    if (params.lowercase) {
        boost::algorithm::to_lower(line);
    }
		//cerr << line << endl;
		
    vector<string> toks;
		 Tokenize(toks, line);
		 for (size_t i = 0; i < toks.size(); ++i) {
      const string &tok = toks[i];
			
			bool found = Decode(tok, params);

  		if ( !found ) {
       if (params.outWords) {
         cout << tok << " ";      
       }
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
	
  if (params.outWords) {
    cout << endl; 
  }
	cout << "totalToks=" << totalToks 
			<< " oovToks=" << oovToks << "(" << tokRatio << ")"
			<< " totalTypes=" << totalTypes
			<< " oovTypes=" << oovTypes.size() << "(" << typeRatio << ")" << endl;
}

bool Compound::Decode(const std::string &tok, const Parameter &params) const
{
  std::unordered_set<size_t> stack;
  stack.insert(0);
  
  while (!stack.empty()) {
    std::unordered_set<size_t>::iterator iter = stack.begin();
    size_t startPos = *iter;
    stack.erase(iter);
    
    bool ret = Decode(stack, tok, startPos, params);
    if (ret) {
      return true;
    }
  }
  
  // still can't find a completed word
  return false;
  
}

bool Compound::Decode(std::unordered_set<size_t> &stack, 
                    const std::string &tok, size_t startPos,
                    const Parameter &params) const
{
  const Node *node = (startPos == 0 ? &m_root : m_rootJuncture);
  
  // start loop
  for (size_t currPos = startPos; currPos < tok.size(); ++currPos) {
    char c = tok[currPos];
    node = node->Find(c);
    
    if (node) {
      if (node->endJuncture) {
        stack.insert(currPos + 1);
      }
    }
    else {
      // couldn't get to the end
      return false;
    }
  }
  
  // got to the end. Is it a word?
  return node->isAWord;
}


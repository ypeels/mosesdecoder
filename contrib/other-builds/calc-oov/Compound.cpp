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

void Compound::CalcOOV(std::ifstream &testStrme) const
{
  size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> oovTypes, foundTypes;
	
	string line;
	vector<string> toks;
	while (getline(testStrme, line)) {
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			
			bool found = m_root.Find(tok);

  		if ( !found ) {
    		std::cout << tok << " not found in myset" << endl;
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
	
	cout << "totalToks=" << totalToks 
			<< " oovToks=" << oovToks << "(" << tokRatio << ")"
			<< " totalTypes=" << totalTypes
			<< " oovTypes=" << oovTypes.size() << "(" << typeRatio << ")" << endl;
}

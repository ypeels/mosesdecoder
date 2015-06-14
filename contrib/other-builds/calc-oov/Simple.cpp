#include <vector>
#include <iostream>
#include "Simple.h"
#include "Main.h"

using namespace std;

void Simple::CreateVocab(std::ifstream &corpusStrme)
{
	string line;
	vector<string> toks;
	while (getline(corpusStrme, line)) {
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			vocab.insert(tok);
		}
		
		toks.clear();
	}
}


void Simple::CalcOOV(std::ifstream &testStrme, std::ofstream &outStream) const
{
	size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> oovTypes, foundTypes;
	
	string line;
	while (getline(testStrme, line)) {
    vector<string> toks;
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			
			std::unordered_set<std::string>::const_iterator got = vocab.find (tok);

  		if ( got == vocab.end() ) {
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

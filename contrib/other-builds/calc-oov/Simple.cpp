#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Simple.h"
#include "Main.h"
#include "Parameter.h"

using namespace std;

Simple::Simple()
{
  cerr << "Simple\n";  
}

void Simple::CreateVocab(std::ifstream &corpusStrme, const Parameter &params)
{
  string line;
	vector<string> toks;
	while (getline(corpusStrme, line)) {
    if (params.lowercase) {
        boost::algorithm::to_lower(line);
    }
    
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			vocab.insert(tok);
		}
		
		toks.clear();
	}
}


void Simple::CalcOOV(std::set<std::string> &oovTypes,
                    std::ifstream &testStrme, const Parameter &params) const
{
	size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> foundTypes;
	  
	string line;
	while (getline(testStrme, line)) {
    if (params.lowercase) {
        boost::algorithm::to_lower(line);
    }

   vector<string> toks;
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			
			std::unordered_set<std::string>::const_iterator got = vocab.find (tok);

  		if ( got == vocab.end() ) {
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

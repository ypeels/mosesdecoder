#include "Main.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

	string corpusPath, testPath;
	corpusPath = argv[1];
	testPath = argv[2];

	// get training data vocab
	ifstream corpusStrme;
	corpusStrme.open(corpusPath.c_str());
	
	unordered_set<string> vocab;
	CreateVocab(vocab, corpusStrme);
	
	corpusStrme.close();
	
	// look up each word in test set
	ifstream testStrme;
	testStrme.open(testPath.c_str());
	
	CalcOOV(testStrme, vocab);

	testStrme.close();
	
	cerr << "finished" << endl;
	return 0;
}

void CreateVocab(std::unordered_set<std::string> &vocab, std::ifstream &corpusStrme)
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

void CalcOOV(std::ifstream &testStrme, const std::unordered_set<std::string> &vocab)
{
	size_t totalToks = 0, oovToks = 0;
	std::unordered_set<std::string> oovTypes, foundTypes;
	
	string line;
	vector<string> toks;
	while (getline(testStrme, line)) {
		Tokenize(toks, line);
		for (size_t i = 0; i < toks.size(); ++i) {
			const string &tok = toks[i];
			
			std::unordered_set<std::string>::const_iterator got = vocab.find (tok);

  		if ( got == vocab.end() ) {
    		//std::cout << tok << " not found in myset" << endl;
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



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
  
}

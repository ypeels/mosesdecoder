#include <iostream>
#include <string>
#include "moses/Util.h"
#include "phrase-extract/InputFileStream.h"
#include "phrase-extract/OutputFileStream.h"

using namespace std;
using namespace Moses;

void Process(const string &line, size_t beginFactor, size_t endFactor, OutputFileStream &outStrme);

int main(int argc, char** argv)
{
  cerr << "Starting...";

  assert(argc == 5);
	size_t beginFactor = Scan<size_t>(argv[1]);
	size_t endFactor = Scan<size_t>(argv[2]);

	string inPath = argv[3];
	string outPath = argv[4];

	InputFileStream inStrme(inPath);
	OutputFileStream outStrme(outPath);

  int lineNum = 0;
	string line;
  while (getline(inStrme, line)) {
		++lineNum;
    if (lineNum%10000 == 0) cerr << "." << flush;

		Process(line, beginFactor, endFactor, outStrme);
  }

  cerr << "Finished." << endl;
}

void Process(const string &line, size_t beginFactor, size_t endFactor, OutputFileStream &outStrme)
{
  size_t maxFactor = max(beginFactor, endFactor);

  // tokenize
	vector<string> toks;
  Tokenize(toks, line);

	vector<vector<string> > tokToks(toks.size());

	for (size_t i = 0; i < toks.size(); ++i) {
    vector<string> factors = Tokenize(toks[i], "|");
		assert(factors.size() - 1 <= maxFactor);

		tokToks[i] = factors;
	}

	// output
	for (size_t i = 1; i < toks.size(); ++i) {
		const vector<string> &prevFactors = tokToks[i-1];
		const vector<string> &thisFactors = tokToks[i];

		outStrme << prevFactors[beginFactor] << " " << thisFactors[endFactor] << endl;
  }
}



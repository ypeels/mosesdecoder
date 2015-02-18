#include <iostream>
#include <string>
#include "moses/Util.h"
#include "phrase-extract/InputFileStream.h"
#include "phrase-extract/OutputFileStream.h"

using namespace std;
using namespace Moses;

void Process(const string &line, OutputFileStream &outStrme);

int main(int argc, char** argv)
{
  cerr << "Starting...";

  assert(argc == 3);

	string inPath = argv[1];
	string outPath = argv[2];

	InputFileStream inStrme(inPath);
	OutputFileStream outStrme(outPath);

  int lineNum = 0;
	string line;
  while (getline(inStrme, line)) {
		++lineNum;
    if (lineNum%10000 == 0) cerr << "." << flush;

		Process(line, outStrme);
  }

  cerr << "Finished." << endl;
}

void Process(const string &line, OutputFileStream &outStrme)
{
  // tokenize
	vector<string> toks;
  Tokenize(toks, line);

	// output
	for (size_t i = 1; i < toks.size(); ++i) {
		const string &prevWord = toks[i-1];
		const string &thisWord = toks[i];

		outStrme << prevWord << " " << thisWord << endl;
  }
}



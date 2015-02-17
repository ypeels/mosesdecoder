#include <iostream>
#include <string>
#include "moses/Util.h"
#include "phrase-extract/InputFileStream.h"
#include "phrase-extract/OutputFileStream.h"

using namespace std;
using namespace Moses;

int main(int argc, char** argv)
{
  assert(argc == 3);

	string inPath = argv[1];
	string outPath = argv[2];

	InputFileStream inStrme(inPath);
	OutputFileStream outStrme(outPath);

	string prevLine, line;
	size_t count = 0;
  while (getline(inStrme, line)) {
		if (prevLine != line) {
			if (!prevLine.empty()) {
				outStrme << count << " " << prevLine << endl;
			}

  		prevLine = line;
			count = 0;
		}

		++count;
  }

	// last
	outStrme << count << " " << prevLine << endl;


}



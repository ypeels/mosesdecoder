#include <cassert>
#include <iostream>
#include <string>
#include <set>
#include <cassert>
#include <stdlib.h>
#include "Util.h"
#include "InputFileStream.h"

using namespace std;

bool OK(const string &line, const vector<string> &origVec);

int main(int argc, char** argv)
{
	cerr << "Starting" << endl;
	assert(argc == 2);

	string origPath = argv[1];
	Moses::InputFileStream origStrme(origPath);
	string origLine;
	getline(origStrme, origLine);
	vector<string> origVec = TokenizeMultiCharSeparator(origLine, "|||");

	string line;
	while (getline(cin, line)) {
		bool ok = OK(line, origVec);

		if (ok) {
			cout << line << endl;

			getline(origStrme, origLine);
			origVec = TokenizeMultiCharSeparator(origLine, "|||");
		}			
	}

	cerr << "Finished" << endl;
}

bool OK(const string &line, const vector<string> &origVec)
{
	std::vector<std::string> toks = TokenizeMultiCharSeparator(line, "|||");
	if (toks[0] == origVec[0] && toks[1] == origVec[1]) {
		return true;
	}
	else {
		return false;
	}
}



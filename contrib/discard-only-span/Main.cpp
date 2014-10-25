#include <iostream>
#include <string>
#include <set>
#include <cassert>
#include <stdlib.h>
#include "Util.h"


using namespace std;

bool OK(const string &line);

int main()
{
	cerr << "Starting" << endl;

	string line;
	while (getline(cin, line)) {
		bool ok = OK(line);

		if (ok) {
			cout << line << endl;
		}			
	}

	cerr << "Finished" << endl;
}

bool OK(const string &line)
{
	std::vector<std::string> toks = TokenizeMultiCharSeparator(line, "|||");
	if (toks.size() < 7) {
		return true;
	}

	typedef set<size_t> Lengths;
	typedef vector<Lengths> SpanLength;
	SpanLength spanLength;

	// find and parse spanlength property
	bool inside = false;

	string propStr = toks[6];
	toks.clear();
	Tokenize(toks, propStr);
 
	for (size_t i = 0; i < toks.size(); ++i) {
		const string &tok = toks[i];
		if (tok == "{{SpanLength") {
			inside = true;
		}
		else if (inside) {
			if (tok == "}}") {
				break;
			}

			// parse string. May be a prob 0.444 or a span length spec 0,1,2
			vector<string> spanLenVec;
			Tokenize(spanLenVec, tok, ",");

			if (spanLenVec.size() == 3) {
				size_t ntInd = atoi(spanLenVec[0].c_str());
				size_t sourceLength = atoi(spanLenVec[1].c_str());

				if (ntInd >= spanLength.size()) {
					spanLength.resize(ntInd + 1);
				}

				Lengths &lengths = spanLength[ntInd];
				lengths.insert(sourceLength);
			}
		}
	} // for

	// ok or not
	/*
	for (size_t i = 0; i < spanLength.size(); ++i) {
		cerr << ", ntInd=" << i << ":";
		const Lengths &lengths = spanLength[i];
		Lengths::const_iterator iter;
		for (iter = lengths.begin(); iter != lengths.end(); ++iter) {
			size_t length = *iter;
			cerr << length << ",";
		}
	}
	*/

	for (size_t i = 0; i < spanLength.size(); ++i) {
		const Lengths &lengths = spanLength[i];

		assert(lengths.size());
		if (lengths.size()  > 1) {
			continue;
		}

		//only 1 span length.
		size_t length = *lengths.begin();
		if (length == 1) {
			return false;
		}

	}

	return true;
}



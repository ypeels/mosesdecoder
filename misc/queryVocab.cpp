#include <iostream>
#include "moses/FF/JoinScore/TrieSearch.h"

using namespace std;
using namespace Moses;

int main(int argc, char* argv[])
{
	cerr << "Starting..." << endl;

  string inPath(argv[1]);
  TrieSearch<bool> trieSearch;
  trieSearch.Create(inPath);
	
	// MAIN LOOP
	string line;
	while (getline(cin, line)) {
  	if (line.empty()) {
			continue;
		}

		bool value;
		bool found = trieSearch.Find(value, line);
		if (found) {
			cerr << "FOUND=" << value << endl;	
		}
		else {
			cerr << "NOT FOUND" << endl;	
		}		
	}
	
	cerr << "Finished" << endl;
}

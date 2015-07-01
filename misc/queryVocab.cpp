#include <iostream>
#include "moses/FF/JoinScore/TrieSearch.h"

using namespace std;
using namespace Moses;

int main(int argc, char* argv[])
{
	cerr << "Starting..." << endl;

  string inPath(argv[1]);
	std::pair<NodeSearch<bool>*, const char *> myPair = NodeSearch<bool>::Create(inPath);
	NodeSearch<bool> &rootNode = *myPair.first;
	const char *data = myPair.second;
	
	// MAIN LOOP
	string line;
	while (getline(cin, line)) {
  	if (line.empty()) {
			continue;
		}

		bool value;
		bool found = rootNode.Find(value, line, 0, data);
		if (found) {
			cerr << "FOUND=" << value << endl;	
		}
		else {
			cerr << "NOT FOUND" << endl;	
		}
		
		rootNode.Clear();
	}
	
	cerr << "Finished" << endl;
}

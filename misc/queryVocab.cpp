#include <iostream>
#include "moses/FF/JoinScore/TrieSearch.h"

using namespace std;
using namespace Moses;

//! delete white spaces at beginning and end of string
inline std::string Trim(const std::string& str, const std::string dropChars = " \t\n\r")
{
  std::string res = str;
  res.erase(str.find_last_not_of(dropChars)+1);
  return res.erase(0, res.find_first_not_of(dropChars));
}

int main(int argc, char* argv[])
{
	cerr << "Starting..." << endl;

  string modelPath(argv[1]);
  TrieSearch<bool> trieSearch;
  trieSearch.Create(modelPath);
	
	// MAIN LOOP
	string line;
	while (getline(cin, line)) {
		line = Trim(line);
  	if (line.empty()) {
			continue;
		}

		cout << line << " ";
		bool value;
		bool found = trieSearch.Find(value, line);
		if (found) {
			//cout << "FOUND=" << value << endl;	
		}
		else {
			//cout << "NOT FOUND" << endl;	
		}
		cout << found << " " << value << endl;
	}
	
	cerr << "Finished" << endl;
}

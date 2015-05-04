#include <iostream>
#include <sstream>
#include "Main.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  vector<string> toks;
  string line;
  while (getline(cin, line)) {
    Tokenize(toks, line);
    string detokLine = ProcessLine(toks);
    cout << detokLine << endl;
    
    toks.clear();
  }
  
	cerr << "finished" << endl;
	return 0;
}

std::string ProcessLine(const std::vector<std::string> &toks)
{
  stringstream ret;
  
  for (size_t i = 0; i < toks.size(); ++i) {
    const string &tok = toks[i];
    if (tok.back() == '+') {
      ret << tok.substr(0, tok.length() - 1);
    }
    else {
      ret << tok << " ";
    }
    
  }

  return ret.str();
}
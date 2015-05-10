#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "Main.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  assert(argc >= 3);
  
  ifstream *inA = new ifstream();
  inA->open(argv[1]);

  ifstream *inB = new ifstream();
  inB->open(argv[2]);

  vector<string> toksA, toksB;
  string lineA, lineB;
  while (getline(*inA, lineA)) {
    getline(*inB, lineB);
    //cerr << "line=" << line << endl;
    Tokenize(toksA, lineA);
    Tokenize(toksB, lineB);
    
    ProcessLine(toksA, toksB);
    
    toksA.clear();
    toksB.clear();
  }
  
  delete inA;
  delete inB;

	cerr << "finished" << endl;
	return 0;
}

std::string ProcessLine(const std::vector<std::string> &toks1, const std::vector<std::string> &toks2)
{

}


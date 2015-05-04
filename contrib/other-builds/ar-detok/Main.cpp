#include <iostream>
#include <fstream>
#include <sstream>
#include "Main.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  // init classes
  CLASSES["w+"] = 1;
  CLASSES["f+"] = 1;
  
  CLASSES["l+"] = 2;
  CLASSES["b+"] = 2;
  CLASSES["k+"] = 2;
  CLASSES["s+"] = 2;
  
  CLASSES["Al+"] = 3;

  CLASSES["+h"] = 5;
  CLASSES["+hA"] = 5;
  CLASSES["+hm"] = 5;
  CLASSES["+hmA"] = 5;
  CLASSES["+hn"] = 5;
  CLASSES["+k"] = 5;
  CLASSES["+km"] = 5;
  CLASSES["+kmA"] = 5;
  CLASSES["+kn"] = 5;
  CLASSES["+nA"] = 5;
  CLASSES["+y"] = 5;

  istream *in;
  if (argc > 1) {
    ifstream *inFile = new ifstream();
    inFile->open(argv[1]);
    in = inFile;
  }
  else {
    in = &cin;
  }
  
  vector<string> toks;
  string line;
  while (getline(*in, line)) {
    //cerr << "line=" << line << endl;
    Tokenize(toks, line);
    string detokLine = ProcessLine(toks);
    cout << detokLine << endl;
    
    toks.clear();
  }
  
  if (in != &cin) {
    delete in;
  }

	cerr << "finished" << endl;
	return 0;
}

std::string ProcessLine(const std::vector<std::string> &toks)
{
  stringstream ret;
  
  int prevStage = 0;
  for (size_t i = 0; i < toks.size(); ++i) {
    const string &tok = toks[i];
    int stage = FindClass(tok);
    //cerr << "tok=" << tok << " prevStage=" << prevStage << " stage=" << stage << endl;
    
    switch (prevStage) {
    case 0:
      switch (stage) {
        case 1:
        case 2:        
        case 3:
          ret << tok.substr(0, tok.length() - 1);
          break;
        case 4:
          ret << tok;
          break;
        case 5:
          abort();
      }
      break;
      
    case 1:
      switch (stage) {
        case 1:
          abort();
        case 2:        
        case 3:
          ret << tok.substr(0, tok.length() - 1);
          break;
        case 4:
          ret << tok;
          break;
        case 5:
          abort();
      }
      break;
    
    case 2:
      switch (stage) {
        case 1:
        case 2:        
          abort();
        case 3:
          ret << tok.substr(0, tok.length() - 1);
          break;
        case 4:
          ret << tok;
          break;
        case 5:
          abort();
      }
      break;

    case 3:
      switch (stage) {
        case 1:
        case 2:        
        case 3:
          abort();
        case 4:
          ret << tok;
          break;
        case 5:
          abort();
      }
      break;
    
    case 4:
      switch (stage) {
        case 1:
        case 2:        
        case 3:
          ret << " ";
          ret << tok.substr(0, tok.length() - 1);
          break;
        case 4:
          ret << " ";
          ret << tok;
          break;
        case 5:
          ret << tok.substr(1, tok.length() - 1);
          break;
      }
      break;
    
    case 5:
      switch (stage) {
        case 1:
        case 2:        
        case 3:
          ret << " ";
          ret << tok.substr(0, tok.length() - 1);
          break;
        case 4:
          ret << " ";
          ret << tok;
          break;
        case 5:
          abort();
      }
      break;
    
    default:
      abort();
    } // outer switch
    
    prevStage = stage;
  } // for

  return ret.str();
}

int FindClass(const std::string &tok)
{
  std::map<std::string, int>::const_iterator iter = CLASSES.find(tok);
  if (iter == CLASSES.end()) {
    return 4;
  }
  else {
    return iter->second;
  }

}
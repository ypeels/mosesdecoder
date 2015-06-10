#include <iostream>
#include "Main.h"
#include "Base.h"
#include "Simple.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

	string corpusPath, testPath;
	corpusPath = argv[1];
	testPath = argv[2];

	// get training data vocab
	ifstream corpusStrme;
	corpusStrme.open(corpusPath.c_str());
	
  Base *obj = new Simple();
  
	obj->CreateVocab(corpusStrme);
	
	corpusStrme.close();
	
	// look up each word in test set
	ifstream testStrme;
	testStrme.open(testPath.c_str());
	
	obj->CalcOOV(testStrme);

	testStrme.close();
	
  delete obj;
  
	cerr << "finished" << endl;
	return 0;
}




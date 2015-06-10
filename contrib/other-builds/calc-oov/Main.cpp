#include <iostream>
#include "Main.h"
#include "Simple.h"
#include "Compound.h"

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
	
  //Base *obj = new Simple();
  Base *obj = new Compound();
  
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




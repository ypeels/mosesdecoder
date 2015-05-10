#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cassert>
#include "Main.h"
#include "LCS.h"

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
    
    std::vector<Point> alignments;
    
    ProcessLine(alignments, toksA, toksB);
    
    for (size_t i = 0; i < alignments.size(); ++i) {
        Point &p = alignments[i];
        cout << p.x << "-" << p.y << " ";
    }
    cout << endl;

    toksA.clear();
    toksB.clear();
  }
  
  delete inA;
  delete inB;

	cerr << "finished" << endl;
	return 0;
}

void ProcessLine(std::vector<Point> &alignments, const std::vector<std::string> &toksA, const std::vector<std::string> &toksB)
{
  std::vector<Point> matches, mismatches;
  
  const string *a = toksA.data();
  const string *b = toksB.data();
  
  LCS::findOne(a, toksA.size(), b, toksB.size(), matches);

  /*
  cerr << "matches: ";
  for (size_t i = 0; i < matches.size(); ++i) {
      Point &p = matches[i];
      cerr << p.x << "-" << p.y << " ";
  }
  */
  
  // mismatches
  int prevX = -1, prevY = -1;
  for (size_t i = 0; i < matches.size(); ++i) {
      Point &p = matches[i];
      if (p.x > (prevX + 1)) {
        int startMismatchX = prevX + 1;
        int endMismatchX = p.x - 1;
        
        int startMismatchY = prevY + 1;
        int endMismatchY = p.y - 1;
        
        CreateMismatches(mismatches, startMismatchX, endMismatchX, startMismatchY, endMismatchY);
      }
      
      prevX = p.x;
      prevY = p.y;
  }

  /*
  cerr << "mismatches: ";
  for (size_t i = 0; i < mm2.size(); ++i) {
      Point &p = mm2[i];
      cerr << p.x << "-" << p.y << " ";
  }
  cerr << endl;
  */
  
  std::copy(matches.begin(), matches.end(), std::inserter(alignments, alignments.end()));
  std::copy(mismatches.begin(), mismatches.end(), std::inserter(alignments, alignments.end()));

}

void CreateMismatches(std::vector<Point> &mismatches, int startMismatchX, int endMismatchX, int startMismatchY, int endMismatchY)
{
  //cerr << "mismatches: (" << startMismatchX << " " << endMismatchX << " " << startMismatchY << " " << endMismatchY << ") ";
  for (int x = startMismatchX; x <= endMismatchX; ++x) {
    for (int y = startMismatchY; y <= endMismatchY; ++y) {
      Point p(x,y);
      mismatches.push_back(p);
    }
  }

}
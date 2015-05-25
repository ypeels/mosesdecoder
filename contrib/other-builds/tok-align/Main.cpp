#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cassert>
#include <boost/program_options.hpp>
#include "Main.h"
#include "LCS.h"
#include "Parameter.h"

using namespace std;

int main(int argc, char **argv)
{
	cerr << "starting" << endl;

  Parameter params;
  
  namespace po = boost::program_options;
  po::options_description desc("Options");
  desc.add_options()
  ("help", "Print help messages")
  ("method", po::value<int>()->default_value(params.method), "Method. 1=LCS(default), 2=char-based")
  ;

    po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc),
              vm); // can throw

    /** --help option
     */
    if ( vm.count("help") || argc < 3 ) {
      std::cout << argv[0] << " target source [options...]" << std::endl
                << desc << std::endl;
      return EXIT_SUCCESS;
    }

    po::notify(vm); // throws on error, so do after help in case
    // there are any problems
  } catch(po::error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return EXIT_FAILURE;
  }

  if (vm.count("method")) params.method = vm["method"].as<int>();


  // BEGIN
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
    
    if (params.method == 1) {
      ProcessLineLCS(alignments, params, toksA, toksB);
    }
    else {
      ProcessLineChar(alignments, params, toksA, toksB);
    }
    
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

void ProcessLineChar(std::vector<Point> &alignments, const Parameter &params, const std::vector<std::string> &toksX, const std::vector<std::string> &toksY)
{
  
}

void ProcessLineLCS(std::vector<Point> &alignments, const Parameter &params, const std::vector<std::string> &toksX, const std::vector<std::string> &toksY)
{
  std::vector<Point> matches, mismatches;
  
  const string *x = toksX.data();
  const string *y = toksY.data();
  
  LCS::findOne(x, toksX.size(), y, toksY.size(), matches);

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

	if (prevX < (int) toksX.size() - 1) {
	  assert(prevY < (int) toksY.size() - 1);
	  
	  int startMismatchX = prevX + 1;
		int endMismatchX = toksX.size() - 1;
		
		int startMismatchY = prevY + 1;
		int endMismatchY = toksY.size() - 1;
        
	  CreateMismatches(mismatches, startMismatchX, endMismatchX, startMismatchY, endMismatchY);
	}

  /*
  cerr << "mismatches: ";
  for (size_t i = 0; i < mismatches.size(); ++i) {
      Point &p = mismatches[i];
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
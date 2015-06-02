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
  ("change-corpus", "Add prefixes and suffixes to splitted words")
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
  if (vm.count("change-corpus")) params.changeCorpus = true;


  // BEGIN
  ifstream *inSource = new ifstream();
  inSource->open(argv[1]);

  ifstream *inTarget = new ifstream();
  inTarget->open(argv[2]);

  // output changed data
  ofstream *outSource;
  if (params.changeCorpus) {
    string changeSource = string(argv[1]) + ".changed";
    outSource = new ofstream();
    outSource->open(changeSource.c_str());
  }
  
  vector<string> toksSource, toksTarget;
  string lineSource, lineTarget;
  size_t lineNum = 1;
  while (getline(*inSource, lineSource)) {
    getline(*inTarget, lineTarget);
    //cerr << "lineSource=" << lineSource << endl;
    //cerr << "lineTarget=" << lineTarget << endl;

    Tokenize(toksSource, lineSource);
    Tokenize(toksTarget, lineTarget);
    
    std::vector<Point> alignments;
    
    if (params.method == 1) {
      ProcessLineLCS(alignments, params, toksSource, toksTarget, lineNum);
    }
    else if (params.method == 2) {
      ProcessLineChar(alignments, params, toksSource, toksTarget, lineNum);
    }
    else {
      abort();
    }
    
    for (size_t i = 0; i < alignments.size(); ++i) {
        Point &p = alignments[i];
        cout << p.x << "-" << p.y << " ";
    }
    cout << endl;

    if (params.changeCorpus) {
        OutputSource(*outSource, toksSource, alignments, params);
    }
    
    toksSource.clear();
    toksTarget.clear();
    
    ++lineNum;
  }
  
  inSource->close();
  inTarget->close();
  delete inSource;
  delete inTarget;

  if (params.changeCorpus) {
    outSource->close();
    delete outSource;
  }

	cerr << "finished" << endl;
	return 0;
}

void OutputSource(ofstream &outSource, const vector<string> &toksSource, 
                const std::vector<Point> &alignments, const Parameter &params)
{
  // compute which source words should have prefixes and suffixes
  typedef pair<bool, bool> PreAndSuff;
  vector<PreAndSuff> preAndSuffs(toksSource.size(), PreAndSuff(false,false));

  // create temp data, sorted by target, source
  typedef map< int, set<int> > AlignT2S;
  AlignT2S alignT2S;
  for (size_t i = 0; i < alignments.size(); ++i) {
    const Point &p = alignments[i];
    int source = p.x;
    int target = p.y;
      
    alignT2S[target].insert(source);
  }
  
  // figure put which source word need prefix and postfix
  AlignT2S::const_iterator iterOuter;
  for (iterOuter = alignT2S.begin(); iterOuter != alignT2S.end(); ++iterOuter) {
    const set<int> &sources = iterOuter->second;
    
    if (sources.size() > 1) {
      set<int>::const_iterator iterInner;      
      for (iterInner = sources.begin(); iterInner != sources.end(); ++iterInner) {
        int source = *iterInner;
        preAndSuffs[source].first = true;    
        preAndSuffs[source].second = true;    
      } 
      
      // 1st & last
      int source = *sources.begin();
      preAndSuffs[source].first = false;

      source = *(--sources.end());
      preAndSuffs[source].second = false;

    }
  }

  // output to file
  for (size_t i = 0; i < toksSource.size(); ++i) {
    const PreAndSuff &preAndSuff = preAndSuffs[i];
    if (preAndSuff.first) {
      outSource << params.prefix;
    }
    outSource << toksSource[i];
    if (preAndSuff.second) {
      outSource << params.suffix;
    }
    outSource << " ";    
  } 
  outSource << endl;
}

void CreateCrossProduct(std::vector<Point> &alignments, const vector<int> &indsSource, const vector<int> &indsTarget)
{
  for (size_t i = 0; i < indsSource.size(); ++i) {
    size_t x = indsSource[i];
    for (size_t j = 0; j < indsTarget.size(); ++j) {
      size_t y = indsTarget[j];
      Point point(x,y);
      alignments.push_back(point);
    }
  }
}

void ProcessLineChar(std::vector<Point> &alignments, const Parameter &params, 
                  const std::vector<std::string> &toksSource, 
                  const std::vector<std::string> &toksTarget,
                  size_t lineNum)
{
  int indSource = 0, indTarget = 0, posSource = 0, posTarget = 0;
  vector<int> indsSource, indsTarget;
  
  //cerr << "toksSource=" << toksSource.size() << " toksTarget=" << toksTarget.size() << endl;
  
  while (true) {
    if (posSource == posTarget) {
      // match
      CreateCrossProduct(alignments, indsSource, indsTarget);
      
      // reset
      indsSource.clear();
      indsTarget.clear();
      
      if (indSource < toksSource.size()) {
        // still more to go
        if (indTarget >= toksTarget.size()) {
          cerr << "Ignoring(A) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
        }
        
        posSource += toksSource[indSource].size();  
        indsSource.push_back(indSource);
      
        posTarget += toksTarget[indTarget].size();    
        indsTarget.push_back(indTarget);
        
        //cerr << "add " << indSource << " " << indTarget << endl;
        
        ++indSource; 
        ++indTarget;
      }
      else {
        // end of sentence
        //assert(indTarget == toksTarget.size());
        if (indTarget != toksTarget.size()) {
            cerr << "Ignoring(B) line " << lineNum << ". Number of characters don't match" << endl;  
            alignments.clear();
            return;
        }

        break;
      }
    } // if (posSource == posTarget) 
    else if (posSource < posTarget) {
      // mismatch
      //assert(indSource < toksSource.size());
      if (indSource >= toksSource.size()) {
          cerr << "Ignoring(C) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
      }

      posSource += toksSource[indSource].size();  
      indsSource.push_back(indSource);
      
      //cerr << "add indSource " << indX << endl;
        
      ++indSource;
    }
    else if (posSource > posTarget) {
      // mismatch
      //assert(indTarget < toksTarget.size())
      if (indTarget >= toksTarget.size()) {
          cerr << "Ignoring(A) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
      }
      
      posTarget += toksTarget[indTarget].size();  
      indsTarget.push_back(indTarget);

      //cerr << "add indTarget " << indTarget << endl;

      ++indTarget;
    }
    else {
      abort();
    }
  }
  
}

void ProcessLineLCS(std::vector<Point> &alignments, 
                    const Parameter &params, 
                    const std::vector<std::string> &toksSource, 
                    const std::vector<std::string> &toksTarget,
                    size_t lineNum)
{
  std::vector<Point> matches, mismatches;
  
  const string *x = toksSource.data();
  const string *y = toksTarget.data();
  
  LCS::findOne(x, toksSource.size(), y, toksTarget.size(), matches);

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

	if (prevX < (int) toksSource.size() - 1) {
	  assert(prevY < (int) toksTarget.size() - 1);
	  
	  int startMismatchX = prevX + 1;
		int endMismatchX = toksSource.size() - 1;
		
		int startMismatchY = prevY + 1;
		int endMismatchY = toksTarget.size() - 1;
        
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


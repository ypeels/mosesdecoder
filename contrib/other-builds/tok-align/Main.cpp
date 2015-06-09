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
  ("junctured-path", po::value<string>()->default_value(params.juncturedPath), "Add prefixes and suffixes to splitted words")
  ;

    po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc),
              vm); // can throw

    /** --help option
     */
    if ( vm.count("help") || argc < 3 ) {
      std::cout << argv[0] << " Unsplit Split [options...]" << std::endl
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
  if (vm.count("junctured-path")) params.juncturedPath = vm["new-split-path"].as<string>();


  // BEGIN
  ifstream *inSplit = new ifstream();
  inSplit->open(argv[1]);

  ifstream *inUnsplit = new ifstream();
  inUnsplit->open(argv[2]);

  // output changed data
  ofstream *outJunctured;
  if (!params.juncturedPath.empty()) {
    outJunctured = new ofstream();
    outJunctured->open(params.juncturedPath.c_str());
  }
  
  vector<string> toksSplit, toksUnsplit;
  string lineSplit, lineUnsplit;
  size_t lineNum = 1;
  while (getline(*inSplit, lineSplit)) {
    getline(*inUnsplit, lineUnsplit);
    //cerr << "lineSplit=" << lineSplit << endl;
    //cerr << "lineUnsplit=" << lineUnsplit << endl;
    //cerr << lineNum << " " << flush;
    
    Tokenize(toksSplit, lineSplit);
    Tokenize(toksUnsplit, lineUnsplit);
    
    std::vector<Point> alignments;
    
    if (params.method == 1) {
      ProcessLineLCS(alignments, params, toksSplit, toksUnsplit, lineNum);
    }
    else if (params.method == 2) {
      ProcessLineChar(alignments, params, toksSplit, toksUnsplit, lineNum);
    }
    else {
      abort();
    }
    
    for (size_t i = 0; i < alignments.size(); ++i) {
        Point &p = alignments[i];
        cout << p.x << "-" << p.y << " ";
    }
    cout << endl;

    if (!params.juncturedPath.empty()) {
        OutputJunctured(*outJunctured, toksSplit, alignments, params);
    }
    
    toksSplit.clear();
    toksUnsplit.clear();
    
    ++lineNum;
  } // while (getline(*inSplit, lineSplit)) {
  
  inSplit->close();
  inUnsplit->close();
  delete inSplit;
  delete inUnsplit;

  if (!params.juncturedPath.empty()) {
    outJunctured->close();
    delete outJunctured;
  }

	cerr << "finished" << endl;
	return 0;
}

void OutputJunctured(ofstream &outJunctured, const vector<string> &toksSplit, 
                const std::vector<Point> &alignments, const Parameter &params)
{
  // compute which Split words should have prefixes and suffixes
  typedef pair<bool, bool> PreAndSuff;
  vector<PreAndSuff> preAndSuffs(toksSplit.size(), PreAndSuff(false,false));

  // create temp data, sorted by Unsplit, Split
  typedef map< int, set<int> > AlignT2S;
  AlignT2S alignT2S;
  for (size_t i = 0; i < alignments.size(); ++i) {
    const Point &p = alignments[i];
    int Split = p.x;
    int Unsplit = p.y;
      
    alignT2S[Unsplit].insert(Split);
  }
  
  // figure put which Split word need prefix and postfix
  AlignT2S::const_iterator iterOuter;
  for (iterOuter = alignT2S.begin(); iterOuter != alignT2S.end(); ++iterOuter) {
    const set<int> &Splits = iterOuter->second;
    
    if (Splits.size() > 1) {
      set<int>::const_iterator iterInner;      
      for (iterInner = Splits.begin(); iterInner != Splits.end(); ++iterInner) {
        int Split = *iterInner;
        preAndSuffs[Split].first = true;    
        preAndSuffs[Split].second = true;    
      } 
      
      // 1st & last
      int Split = *Splits.begin();
      preAndSuffs[Split].first = false;

      Split = *(--Splits.end());
      preAndSuffs[Split].second = false;

    }
  }

  // output to file
  for (size_t i = 0; i < toksSplit.size(); ++i) {
    const PreAndSuff &preAndSuff = preAndSuffs[i];
    if (preAndSuff.first) {
      outJunctured << params.prefix;
    }
    outJunctured << toksSplit[i];
    if (preAndSuff.second) {
      outJunctured << params.suffix;
    }
    outJunctured << " ";    
  } 
  outJunctured << endl;
}

void CreateCrossProduct(std::vector<Point> &alignments, const vector<int> &indsSplit, const vector<int> &indsUnsplit)
{
  for (size_t i = 0; i < indsSplit.size(); ++i) {
    size_t x = indsSplit[i];
    for (size_t j = 0; j < indsUnsplit.size(); ++j) {
      size_t y = indsUnsplit[j];
      Point point(x,y);
      alignments.push_back(point);
    }
  }
}

void ProcessLineChar(std::vector<Point> &alignments, const Parameter &params, 
                  const std::vector<std::string> &toksSplit, 
                  const std::vector<std::string> &toksUnsplit,
                  size_t lineNum)
{
  int indSplit = 0, indUnsplit = 0, posSplit = 0, posUnsplit = 0;
  vector<int> indsSplit, indsUnsplit;
  
  //cerr << "toksSplit=" << toksSplit.size() << " toksUnsplit=" << toksUnsplit.size() << endl;
  
  while (true) {
    if (posSplit == posUnsplit) {
      // match
      CreateCrossProduct(alignments, indsSplit, indsUnsplit);
      
      // reset
      indsSplit.clear();
      indsUnsplit.clear();
      
      if (indSplit < toksSplit.size()) {
        // still more to go
        if (indUnsplit >= toksUnsplit.size()) {
          cerr << "Ignoring(A) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
        }
        
        posSplit += toksSplit[indSplit].size();  
        indsSplit.push_back(indSplit);
      
        posUnsplit += toksUnsplit[indUnsplit].size();    
        indsUnsplit.push_back(indUnsplit);
        
        //cerr << "add " << indSplit << " " << indUnsplit << endl;
        
        ++indSplit; 
        ++indUnsplit;
      }
      else {
        // end of sentence
        //assert(indUnsplit == toksUnsplit.size());
        if (indUnsplit != toksUnsplit.size()) {
            cerr << "Ignoring(B) line " << lineNum << ". Number of characters don't match" << endl;  
            alignments.clear();
            return;
        }

        break;
      }
    } // if (posSplit == posUnsplit) 
    else if (posSplit < posUnsplit) {
      // mismatch
      //assert(indSplit < toksSplit.size());
      if (indSplit >= toksSplit.size()) {
          cerr << "Ignoring(C) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
      }

      posSplit += toksSplit[indSplit].size();  
      indsSplit.push_back(indSplit);
      
      //cerr << "add indSplit " << indX << endl;
        
      ++indSplit;
    }
    else if (posSplit > posUnsplit) {
      // mismatch
      //assert(indUnsplit < toksUnsplit.size())
      if (indUnsplit >= toksUnsplit.size()) {
          cerr << "Ignoring(A) line " << lineNum << ". Number of characters don't match" << endl;  
          alignments.clear();
          return;
      }
      
      posUnsplit += toksUnsplit[indUnsplit].size();  
      indsUnsplit.push_back(indUnsplit);

      //cerr << "add indUnsplit " << indUnsplit << endl;

      ++indUnsplit;
    }
    else {
      abort();
    }
  }
  
}

void ProcessLineLCS(std::vector<Point> &alignments, 
                    const Parameter &params, 
                    const std::vector<std::string> &toksSplit, 
                    const std::vector<std::string> &toksUnsplit,
                    size_t lineNum)
{
  std::vector<Point> matches, mismatches;
  
  const string *x = toksSplit.data();
  const string *y = toksUnsplit.data();
  
  LCS::findOne(x, toksSplit.size(), y, toksUnsplit.size(), matches);

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

	if (prevX < (int) toksSplit.size() - 1) {
	  assert(prevY < (int) toksUnsplit.size() - 1);
	  
	  int startMismatchX = prevX + 1;
		int endMismatchX = toksSplit.size() - 1;
		
		int startMismatchY = prevY + 1;
		int endMismatchY = toksUnsplit.size() - 1;
        
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


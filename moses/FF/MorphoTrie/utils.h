#include "MorphTrie.h"
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include "moses/Util.h"

using namespace std;

inline void LoadLm(string lmPath, MorphTrie<string, float>* root) {
  ifstream infile(lmPath.c_str());
	string line;
	char c = '\t';
	while (getline(infile, line)) {
		string::size_type i = 0;
	    string::size_type j = line.find(c);

	    vector<string> substrings;
	    while (j != string::npos) {
	        substrings.push_back(line.substr(i, j-i));
	        i = ++j;
	        j = line.find(c, j);

	        if (j == string::npos)
	        	substrings.push_back(line.substr(i, line.length()));
	    }

	    if (substrings.size() < 2)
	   		continue;

	    float weight = Moses::Scan<float>(substrings[0]);
        
        char d = ' ';
        string ngram = substrings[1];
        i = 0;
        j = ngram.find(c);
	    
        vector<string> key;
        while (j != string::npos) {
            key.push_back(ngram.substr(i, j-i));
            i = ++j;
            j = ngram.find(d, j);

            if (j == string::npos)
                key.push_back(ngram.substr(i, ngram.length()));
        }

        float backoff = 0.f;
        if (substrings.size() == 3)
	  backoff = Moses::Scan<float>(substrings[2]);


	   reverse(key.begin(), key.end());
	   root->insert(key, weight, backoff);
	}

}

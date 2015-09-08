#include <MorphTrie.h>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

void LoadLm(string lmPath, MorphTrie<string, float>& root) {
	ifstream infile(lmPath);
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

	    float weight = stof(substrings[0]);
        
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
            backoff = stof(substrings[2]);

	   
	   root.insert(key, weight, backoff);
	}

}

int main() {
    MorphTrie<string, float>* root = new MorphTrie<string, float>;
    LoadLm("lm_small", *root);
    return 0;
}
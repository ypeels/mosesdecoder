#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include "moses/FF/JoinScore/Trie.h"

using namespace std;
using namespace Moses;

int main(int argc, char* argv[])
{
	cerr << "Starting..." << endl;

  string inPath(argv[1]);
	boost::iostreams::mapped_file_source file;
	file.open(inPath.c_str());
	UTIL_THROW_IF(!file.is_open(),
									 util::FileOpenException,
									 std::string("Couldn't open file ") + inPath);

	size_t size = file.size();
	cerr << "size=" << size << endl;
	
	const char *data = file.data();
	
	uint64_t rootPos = size - sizeof(uint64_t);
	cerr << "BEFORE rootPos=" << rootPos << endl;
	
	const uint64_t *ptr = (const uint64_t*) (data + rootPos);
	rootPos = ptr[0];
	NodeSearch<bool> rootNode(data, rootPos);
	
	cerr << "AFTER rootPos=" << rootPos << endl;
	
	// MAIN LOOP
	string line;
	while (getline(cin, line)) {
  	if (line.empty()) {
			continue;
		}

		bool value;
		bool found = rootNode.Find(value, line, 0, data);
		if (found) {
			cerr << "FOUND=" << value << endl;	
		}
		else {
			cerr << "NOT FOUND" << endl;	
		}
	}
	
	cerr << "Finished" << endl;
}

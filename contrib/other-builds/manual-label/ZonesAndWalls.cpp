#include <cassert>
#include <stack>
#include "ZonesAndWalls.h"

using namespace std;

void ZonesAndWalls(const Phrase &source, std::ostream &out)
{
	stack<size_t> startStack;
	typedef pair<size_t,size_t> Zone;
	vector<Zone> zones;

	size_t size = source.size();
	for (size_t i = 0; i < size; ++i) {
		const Word &word = source[i];
		assert(word.size() == 1);
		const string factor = word[0];

		if (factor == "(") {
			startStack.push(i);
		}
		else if (factor == ")") {
			if (!startStack.empty()) {
				size_t startPos = startStack.top();
				startStack.pop();

				Zone zone(startPos, i);
				zones.push_back(zone);
			}
		}
	}

	// output zones
	for (size_t i = 0; i < zones.size(); ++i) {
		const Zone &zone = zones[i];
		cerr << zone.first << " " << zone.second << " ";
	}
	cerr << endl;
}


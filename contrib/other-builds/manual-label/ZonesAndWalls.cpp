#include <cassert>
#include <stack>
#include <set>
#include "ZonesAndWalls.h"

using namespace std;

void ZonesAndWalls(const Phrase &source, std::ostream &out)
{
	ZonesAndWalls(source, out, "&quot;", "&quot;");
}

void ZonesAndWalls(const Phrase &source, std::ostream &out, const std::string &start, const std::string &end)
{
	stack<size_t> startStack;
	typedef pair<size_t,size_t> Zone;
	typedef set<Zone> Zones;
	Zones zones;

	size_t size = source.size();
	for (size_t i = 0; i < size; ++i) {
		const Word &word = source[i];
		assert(word.size() == 1);
		const string factor = word[0];

		if (start == end) {
			if (factor ==  start) {
				if (startStack.empty()) {
					startStack.push(i);
				}
				else {
					size_t startPos = startStack.top();
					startStack.pop();

					Zone zone(startPos, i);
					zones.insert(zone);
				}
			}
		}
		else {
			if (factor == start) {
				startStack.push(i);
			}
			else if (factor == end) {
				if (!startStack.empty()) {
					size_t startPos = startStack.top();
					startStack.pop();

					Zone zone(startPos, i);
					zones.insert(zone);
				}
			}
		}
	}

	// output
	for (size_t i = 0; i < size; ++i) {
		// before outputting word
		Zones::const_iterator iter;
		for (iter = zones.begin(); iter != zones.end(); ++iter) {
			const Zone &zone = *iter;

			if (zone.first == i) {
				out << "<zone> ";
			}
			if (zone.second == i) {
				out << "<wall /> ";
			}

		}

		const Word &word = source[i];
		assert(word.size() == 1);
		const string factor = word[0];

		out << factor << " ";

		// before outputting word
		for (iter = zones.begin(); iter != zones.end(); ++iter) {
			const Zone &zone = *iter;

			if (zone.first == i) {
				out << "<wall /> ";
			}
			if (zone.second == i) {
				out << "</zone> ";
			}

		}

	}

	out << endl;
}


#include <cassert>
#include <stack>
#include "ZonesAndWalls.h"

using namespace std;

void ZonesAndWalls(const Phrase &source, std::ostream &out)
{
	Zones zonesAndWalls;

	ZonesAndWalls(source, out, "&quot;", "&quot;", zonesAndWalls);
	ZonesAndWalls(source, out, "(", ")", zonesAndWalls);
	ZonesAndWalls(source, out, "&#91;", "&#93;", zonesAndWalls);

	// output
	size_t size = source.size();
	for (size_t i = 0; i < size; ++i) {
		// before outputting word
		Zones::const_iterator iter;
		for (iter = zonesAndWalls.begin(); iter != zonesAndWalls.end(); ++iter) {
			const RangeZoneWall &zone = *iter;

			if (!zone.doWall && zone.doZone) {
				if (zone.range.first == i) {
					out << "<zone> ";
				}
			}
			else if (zone.doWall && zone.doZone) {
				if (zone.range.first == i) {
					out << "<zone> ";
				}
				if (zone.range.second == i) {
					out << "<wall /> ";
				}
			}
		}

		const Word &word = source[i];
		assert(word.size() == 1);
		const string factor = word[0];

		out << factor << " ";

		// before outputting word
		for (iter = zonesAndWalls.begin(); iter != zonesAndWalls.end(); ++iter) {
			const RangeZoneWall &zone = *iter;

			if (!zone.doWall && zone.doZone) {
				if (zone.range.second == i) {
					out << "</zone> ";
				}
			}
			else if (zone.doWall && zone.doZone) {
				if (zone.range.first == i) {
					out << "<wall /> ";
				}
				if (zone.range.second == i) {
					out << "</zone> ";
				}
			}
		}

	}

	out << endl;

}

void ZonesAndWalls(const Phrase &source, std::ostream &out,
		const std::string &start,
		const std::string &end,
		Zones &zonesAndWalls)
{
	stack<size_t> startStack;

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

					RangeZoneWall zone(startPos, i, true, true);
					zonesAndWalls.push_back(zone);
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

					RangeZoneWall zone(startPos, i, true, true);
					zonesAndWalls.push_back(zone);
				}
			}
		}
	}

}


#include <cassert>
#include <stack>
#include "ZonesAndWalls.h"

using namespace std;

void ZonesAndWalls(const Phrase &source, std::ostream &out)
{
	AllRanges allRanges;

	Walls(source, ":", allRanges);
	Walls(source, ";", allRanges);
	Walls(source, "-", allRanges);
	ZonesAndWalls(source, "&quot;", "&quot;", allRanges);
	ZonesAndWalls(source, "(", ")", allRanges);
	ZonesAndWalls(source, "&#91;", "&#93;", allRanges);

	// output
	size_t size = source.size();
	for (size_t i = 0; i < size; ++i) {
		// before outputting word
		AllRanges::const_iterator iter;
		for (iter = allRanges.begin(); iter != allRanges.end(); ++iter) {
			const RangeZoneWall &zone = *iter;

			if (zone.doWall && !zone.doZone) {
				if (zone.range.first == i) {
					out << "<wall /> ";
				}
			}
			else if (!zone.doWall && zone.doZone) {
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

		// after outputting word
		for (iter = allRanges.begin(); iter != allRanges.end(); ++iter) {
			const RangeZoneWall &zone = *iter;

			if (zone.doWall && !zone.doZone) {
				if (zone.range.first == i) {
					out << "<wall /> ";
				}
			}
			else if (!zone.doWall && zone.doZone) {
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

void Walls(const Phrase &source,
		const std::string &sought,
		AllRanges &allRanges)
{
	stack<size_t> startStack;

	size_t size = source.size();
	for (size_t i = 0; i < size; ++i) {
		const Word &word = source[i];
		assert(word.size() == 1);
		const string factor = word[0];

		if (factor ==  sought) {
			RangeZoneWall zone(i, i, true, false);
			allRanges.push_back(zone);
		}
	}
}

void Zones(const Phrase &source,
		const std::string &sought,
		AllRanges &zonesAndWalls)
{

}

void ZonesAndWalls(const Phrase &source,
		const std::string &start,
		const std::string &end,
		AllRanges &allRanges)
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
					allRanges.push_back(zone);
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
					allRanges.push_back(zone);
				}
			}
		}
	}

}


#pragma once

#include "Main.h"

struct RangeZoneWall : public Range
{
	RangeZoneWall(int start,int end, bool w, bool z)
	:Range(start, end)
	,doWall(w)
	,doZone(z)
	{}

	bool doWall, doZone;
};

typedef std::vector<RangeZoneWall> AllRanges;

void ZonesAndWalls(const Phrase &source, std::ostream &out);
void ZonesAndWalls(const Phrase &source,
		const std::string &start,
		const std::string &end,
		AllRanges &zonesAndWalls);
void Walls(const Phrase &source,
		const std::string &sought,
		AllRanges &zonesAndWalls);
void Zones(const Phrase &source,
		const std::string &sought,
		AllRanges &zonesAndWalls);

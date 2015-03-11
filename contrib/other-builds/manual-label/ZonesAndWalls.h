#pragma once

#include "Main.h"

typedef std::pair<size_t,size_t> Zone;
typedef std::set<Zone> Zones;

void ZonesAndWalls(const Phrase &source, std::ostream &out);
void ZonesAndWalls(const Phrase &source, std::ostream &out,
		const std::string &start,
		const std::string &end,
		Zones &zones);

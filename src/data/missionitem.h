#ifndef MISSIONITEM_H
#define MISSIONITEM_H

#include "common/types.h"

namespace data
{

struct MissionItem
{
	int cmd;
	float param1;
	float param2;
	float param3;
	float param4;
	double lat;
	double lon;
	double alt;
};

typedef std::vector<MissionItem> MissionItemList;
typedef std::vector<MissionItem>::iterator MissionItemListIterator;
typedef std::vector<MissionItem>::const_iterator MissionItemListConstIterator;

}

#endif // MISSIONITEM_H

#ifndef MISSIONITEM_H
#define MISSIONITEM_H

#include "icoordinate.h"
#include <mavlink/common/mavlink.h>
#include <vector>

namespace data
{

struct MissionItem
{
	int cmd;
	float param1;
	float param2;
	float param3;
	float param4;
	CoordinatePtr point;
};

typedef std::vector<MissionItem> MissionItemList;
typedef std::vector<MissionItem>::iterator MissionItemListIterator;
typedef std::vector<MissionItem>::const_iterator MissionItemListConstIterator;

}

#endif // MISSIONITEM_H

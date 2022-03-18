#ifndef IMISSION_H
#define IMISSION_H

#include "common/types.h"

namespace data
{

// Mission interface
class IMission
{
public:
	enum class DrawMode
	{
		Points,
		Polygon
	};

public:
	IMission() {}
};

typedef std::shared_ptr<IMission> MissionPtr;

}

#endif // IMISSION_H

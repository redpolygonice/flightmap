#ifndef IMISSION_H
#define IMISSION_H

#include "common/types.h"
#include "data/coordinate.h"
#include "common/anymap.h"
#include "data/missionitem.h"

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
	virtual ~IMission() {}

public:
	virtual bool processMessage(const mavlink_message_t &message) = 0;
	virtual bool createPoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) = 0;
	virtual void createCommands(const CoordinateList &points, MissionItemList &items) = 0;
	virtual void write(const data::CoordinateList &points, const common::AnyMap &params) = 0;
	virtual void read(data::CoordinateList &points) = 0;
	virtual void clear() = 0;
	virtual void setCapabilities(uint64_t capabilities) {}
};

typedef std::shared_ptr<IMission> MissionPtr;

}

#endif // IMISSION_H

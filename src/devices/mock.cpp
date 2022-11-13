#include "mock.h"
#include "common/log.h"
#include "data/mission.h"

namespace device
{

Mock::Mock(const comms::CommunicationPtr &comm)
	: IDevice(comm)
{
	_type = common::DeviceType::Mock;
}

std::string Mock::name() const
{
	return _comm->connectParams().device;
}

Mock::~Mock()
{
}

bool Mock::start()
{
	_telebox = data::TeleBox::create();
	return true;
}

void Mock::stop()
{
}

void Mock::sendCommand(uint16_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
}

void Mock::sendCommandInt(uint16_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{

}

bool Mock::createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	if (_mission == nullptr)
		_mission = data::Mission::create(shared_from_this());

	return _mission->createPoints(points, mission, params);
}

void Mock::clearMission()
{
}

void Mock::writeMission(const data::CoordinateList &points, const common::AnyMap &params)
{
}

void Mock::readMission(data::CoordinateList &points)
{
}

}

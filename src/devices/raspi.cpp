#include "raspi.h"

namespace device
{

Raspi::Raspi(const comms::CommunicationPtr &comm)
	: IDevice(comm)
{
	_type = common::DeviceType::Raspi;
}

device::Raspi::~Raspi()
{
}

bool Raspi::start()
{
	return false;
}

void Raspi::stop()
{
}

void Raspi::sendCommand(uint16_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
}

void Raspi::sendCommandInt(uint16_t cmd, float param1, float param2, float param3, float param4, float x, float y, float z)
{

}

bool Raspi::createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	return false;
}

void Raspi::clearMission()
{
}

void Raspi::writeMission(const data::CoordinateList &points, const common::AnyMap &params)
{
}

void Raspi::readMission(data::CoordinateList &points)
{
}

}

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

void Raspi::sendCommand(uint32_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
}

}

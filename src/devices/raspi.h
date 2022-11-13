#ifndef RASPI_H
#define RASPI_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"

namespace device
{

// Raspberry Pi device
class Raspi : public IDevice
{
private:
	std::atomic_bool _active;

public:
	Raspi(const comms::CommunicationPtr &comm);
	virtual ~Raspi();

public:
	static DevicePtr create(const comms::CommunicationPtr &comm) { return std::make_shared<Raspi>(comm); }
	bool start() override;
	void stop() override;
	void sendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) override;
	void sendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
								 float x = 0, float y = 0, float z = 0) override;
	bool createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) override;
	void clearMission() override;
	void writeMission(const data::CoordinateList &points, const common::AnyMap &params) override;
	void readMission(data::CoordinateList &points) override;
};

}

#endif // RASPI_H

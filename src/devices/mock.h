#ifndef MOCK_H
#define MOCK_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"

namespace device
{

// Mock device
class Mock : public IDevice, public std::enable_shared_from_this<Mock>
{
public:
	Mock(const comms::CommunicationPtr &comm);
	virtual ~Mock();

	Mock(const Mock&) = delete;
	Mock(Mock&&) = delete;
	Mock& operator=(const Mock&) = delete;
	Mock& operator=(Mock&&) = delete;

public:
	static DevicePtr create(const comms::CommunicationPtr &comm) { return std::make_shared<Mock>(comm); }
	string name() const override;
	bool start() override;
	void stop() override;
	void sendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) override;
	void sendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
								 float param5 = 0, float param6 = 0, float param7 = 0) override;
	bool createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) override;
	void clearMission() override;
	void writeMission(const data::CoordinateList &points, const common::AnyMap &params) override;
	void readMission(data::CoordinateList &points) override;
};

}

#endif // MOCK_H

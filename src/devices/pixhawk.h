#ifndef PIXHAWK_H
#define PIXHAWK_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"
#include "data/missionitem.h"

namespace device
{

// Pixhawk device
class Pixhawk : public IDevice, public std::enable_shared_from_this<Pixhawk>
{
private:
	std::atomic_bool _active;
	std::thread _heartbeatThread;
	std::thread _readThread;
	std::thread _processThread;

	std::mutex _mutexData;
	std::vector<unsigned char> _data;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

	message::Factory _messageFactory;

public:
	Pixhawk(const comms::CommunicationPtr &comm);
	virtual ~Pixhawk();

	Pixhawk(const Pixhawk&) = delete;
	Pixhawk(Pixhawk&&) = delete;
	Pixhawk& operator=(const Pixhawk&) = delete;
	Pixhawk& operator=(Pixhawk&&) = delete;

public:
	static DevicePtr create(const comms::CommunicationPtr &comm) { return std::make_shared<Pixhawk>(comm); }
	string name() const override;
	bool start() override;
	void stop() override;
	bool sendMessage(mavlink_message_t &message) override;
	void sendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
					 float param5 = 0, float param6 = 0, float param7 = 0) override;
	void sendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
								 float x = 0, float y = 0, float z = 0) override;
	bool createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) override;
	void clearMission() override;
	void writeMission(const data::CoordinateList &points, const common::AnyMap &params) override;
	void readMission(data::CoordinateList &points) override;
	void RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4, uint16_t ch5 = UINT16_MAX,
			uint16_t ch6 = UINT16_MAX) override;
	void setMode(uint32_t mode) override;
	void setHome(float lat, float lon, float alt) override;
	void setPosition(double lat, double lon, double alt) override;

private:
	void startHeartbeat();
	bool requestData();
	void readData(const ByteArray &data);
	bool processData();
};

}

#endif // PIXHAWK_H

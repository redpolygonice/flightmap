#ifndef PIXHAWK_H
#define PIXHAWK_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"

namespace device
{

// Pixhawk device
class Pixhawk : public IDevice, public std::enable_shared_from_this<Pixhawk>
{
public:
	enum FlightModes : uint32_t
	{
		MODE_STABILIZE = 0,
		MODE_ACRO = 1,
		MODE_ALT_HOLD = 2,
		MODE_AUTO = 3,
		MODE_GUIDED = 4,
		MODE_LOITER = 5,
		MODE_RTL = 6,
		MODE_CIRCLE = 7,
		MODE_LAND = 9,
		MODE_DRIFT = 11,
		MODE_SPORT = 13,
		MODE_FLIP = 14,
		MODE_AUTOTUNE = 15,
		MODE_POSHOLD = 16,
		MODE_BRAKE = 17,
		MODE_THROW = 18,
	};

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
	void sendCommand(uint32_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
					 float param5 = 0, float param6 = 0, float param7 = 0) override;

private:
	bool sendMessage(mavlink_message_t &message);
	void startHeartbeat();
	bool requestData();
	void readData(const ByteArray &data);
	bool processData();
};

}

#endif // PIXHAWK_H

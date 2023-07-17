#ifndef PIXHAWK_H
#define PIXHAWK_H

#include "common/types.h"
#include "devices/flightdevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"

namespace device
{

// Pixhawk device
class Pixhawk : public FlightDevice
{
private:
	std::atomic_bool _active;
	std::thread _heartbeatThread;
	std::thread _processThread;
	message::Factory _messageFactory;

	std::mutex _mutexData;
	std::vector<unsigned char> _data;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

public:
	Pixhawk(const comms::CommunicationPtr &comm);
	virtual ~Pixhawk();

	Pixhawk(const Pixhawk&) = delete;
	Pixhawk(Pixhawk&&) = delete;
	Pixhawk& operator=(const Pixhawk&) = delete;
	Pixhawk& operator=(Pixhawk&&) = delete;

public:
	static DevicePtr Create(const comms::CommunicationPtr &comm) { return std::make_shared<Pixhawk>(comm); }
	string Name() const override;
	bool Start() override;
	void Stop() override;
	bool WaitHeartbeat() override;

private:
	void StartHeartbeat();
	bool RequestData();
	void ReadData(const ByteArray &data);
	bool ProcessData();
};

}

#endif // PIXHAWK_H

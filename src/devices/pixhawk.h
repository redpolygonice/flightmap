#ifndef PIXHAWK_H
#define PIXHAWK_H

#include "common/types.h"
#include "devices/flightdevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"
#include "common/threadsafequeue.h"

namespace device
{

// Pixhawk device
class Pixhawk : public FlightDevice
{
private:
	std::atomic_bool _active;
	std::thread _heartbeatThread;
	std::thread _processThread;
	common::ThreadSafeQueue<ByteArrayPtr> _data;
	message::Factory _messageFactory;

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

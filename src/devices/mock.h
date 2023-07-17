#ifndef MOCK_H
#define MOCK_H

#include "common/types.h"
#include "devices/flightdevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"
#include "common/threadsafequeue.h"

namespace device
{

// Mock device
class Mock : public FlightDevice
{
private:
	std::atomic_bool _active;
	std::thread _processThread;
	common::ThreadSafeQueue<ByteArrayPtr> _data;
	message::Factory _messageFactory;

private:
	void ProcessData();
	void ReadData(const ByteArray &data);

public:
	Mock(const comms::CommunicationPtr &comm);
	virtual ~Mock();

	Mock(const Mock&) = delete;
	Mock(Mock&&) = delete;
	Mock& operator=(const Mock&) = delete;
	Mock& operator=(Mock&&) = delete;

public:
	static DevicePtr Create(const comms::CommunicationPtr &comm) { return std::make_shared<Mock>(comm); }
	string Name() const override;
	bool Start() override;
	void Stop() override;
};

}

#endif // MOCK_H

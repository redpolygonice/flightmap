#ifndef RASPI_H
#define RASPI_H

#include "common/types.h"
#include "devices/flightdevice.h"
#include "comms/icommunication.h"
#include "messages/factory.h"

namespace device
{

// Raspberry Pi device
class Raspi : public FlightDevice
{
private:
	std::atomic_bool _active;
	std::thread _heartbeatThread;
	std::thread _processThread;
	message::Factory _messageFactory;

	std::vector<unsigned char> _data;
	std::mutex _mutexData;

public:
	Raspi(const comms::CommunicationPtr &comm);
	virtual ~Raspi();

	Raspi(const Raspi&) = delete;
	Raspi(Raspi&&) = delete;
	Raspi& operator=(const Raspi&) = delete;
	Raspi& operator=(Raspi&&) = delete;

public:
	static DevicePtr Create(const comms::CommunicationPtr &comm) { return std::make_shared<Raspi>(comm); }
	string Name() const override;
	bool Start() override;
	void Stop() override;
	bool WaitHeartbeat() override;
	void StartCamera() override;
	void StopCamera() override;

private:
	void StartHeartbeat();
	bool RequestData();
	void ReadData(const ByteArray &data);
	bool ProcessData();
};

}

#endif // RASPI_H

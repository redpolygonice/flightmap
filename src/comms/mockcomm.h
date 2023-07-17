#ifndef MOCKCOMM_H
#define MOCKCOMM_H

#include "common/types.h"
#include "icommunication.h"
#include "devices/simulator.h"

namespace comms
{

// Mock communication implementation
class MockComm : public ICommunication
{
private:
	bool _active;
	std::thread _thread;
	ReadFunction _callback;
	device::Simulator _simulator;

public:
	MockComm(const common::ConnectParams &params);
	virtual ~MockComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<MockComm>(params); }
	bool Open() override;
	bool Open(const string &host, unsigned short port) override;
	void Close() override;
	void Start(const ReadFunction &callback) override;
	bool Write(const unsigned char *buffer, size_t size) override;
};

}

#endif // MOCKCOMM_H

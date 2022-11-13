#ifndef MOCKCOMM_H
#define MOCKCOMM_H

#include "common/types.h"
#include "icommunication.h"

namespace comms
{

// Mock communication implementation
class MockComm : public ICommunication
{
private:
	bool _active;
	ReadFunction _callback;
	std::thread _thread;
	std::mutex _mutex;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

private:
	void run();

public:
	MockComm(const common::ConnectParams &params);
	virtual ~MockComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<MockComm>(params); }
	bool open() override;
	bool open(const string &host, unsigned short port) override;
	void close() override;
	void start(const ReadFunction &callback) override;
	bool write(const unsigned char *buffer, size_t size) override;
};

}

#endif // MOCKCOMM_H

#ifndef UARTCOMM_H
#define UARTCOMM_H

#include "common/types.h"
#include "icommunication.h"

#ifdef WIN32
#include <windows.h>
#endif

namespace comms
{

// UART communication implementation
class UartComm : public ICommunication
{
private:
	bool _active;
#ifdef WIN32
	HANDLE _handle;
#else
	int _handle;
#endif
	ReadFunction _callback;
	std::thread _thread;
	std::mutex _mutex;

private:
	void run();

public:
	UartComm(const common::ConnectParams &params);
	virtual ~UartComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<UartComm>(params); }
	bool open() override;
	bool open(const string &host, unsigned short port) override { return false; }
	void close() override;
	void start(const ReadFunction &callback) override;
	bool write(const unsigned char *buffer, size_t size) override;
#ifndef WIN32
	void setBaudRate(uint32_t rate) override;
#endif
};

}

#endif // UARTCOMM_H

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
	std::atomic_bool _active;
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
	bool Open() override;
	bool Open(const string &host, unsigned short port) override { return false; }
	void Close() override;
	void Start(const ReadFunction &callback) override;
	bool Write(const unsigned char *buffer, size_t size) override;
#ifndef WIN32
	void SetBaudRate(uint32_t rate) override;
#endif
};

}

#endif // UARTCOMM_H

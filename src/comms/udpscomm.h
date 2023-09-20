#ifndef UDPSCOMM_H
#define UDPSCOMM_H

#include "common/types.h"
#include "icommunication.h"
#include "isocket.h"

namespace comms
{

// UDP incoming communication implementation
class UdpsComm : public ICommunication
{
private:
	std::atomic_bool _active;
	SocketPtr _socket;
	ReadFunction _callback;
	std::thread _thread;
	std::mutex _mutex;

private:
	void run();

public:
	UdpsComm(const common::ConnectParams &params);
	virtual ~UdpsComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<UdpsComm>(params); }
	bool Open() override;
	bool Open(const string &host, unsigned short port) override;
	void Close() override;
	void Start(const ReadFunction &callback) override;
	bool Write(const unsigned char *buffer, size_t size) override;
};

}

#endif // UDPSCOMM_H

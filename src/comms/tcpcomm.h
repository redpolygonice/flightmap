#ifndef TCPCOMM_H
#define TCPCOMM_H

#include "common/types.h"
#include "icommunication.h"

namespace comms
{

// TCP client communication implementation
class TcpComm : public ICommunication
{
public:
	TcpComm(const common::ConnectParams &params);
	virtual ~TcpComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<TcpComm>(params); }
	bool open() override { return false; }
	bool open(const string &host, unsigned short port) override;
	void close() override;
	void start(const ReadFunction &callback) override;
	bool write(const unsigned char *buffer, size_t size) override;
};

}

#endif // TCPCOMM_H

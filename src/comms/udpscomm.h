#ifndef UDPSCOMM_H
#define UDPSCOMM_H

#include "common/types.h"
#include "icommunication.h"

namespace comms
{

// UDP server communication implementation
class UdpsComm : public ICommunication
{
public:
	UdpsComm(const common::ConnectParams &params);
	virtual ~UdpsComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<UdpsComm>(params); }
	bool open() override { return false; }
	bool open(const string &host, unsigned short port) override;
	void close() override;
	void start(const ReadFunction &callback) override;
	bool write(const unsigned char *buffer, size_t size) override;
};

}

#endif // UDPSCOMM_H

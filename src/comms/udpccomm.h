#ifndef UDPCCOMM_H
#define UDPCCOMM_H

#include "common/types.h"
#include "icommunication.h"

namespace comms
{

// UDP client communication implementation
class UdpcComm : public ICommunication
{
public:
	UdpcComm(const common::ConnectParams &params);
	virtual ~UdpcComm();

public:
	static CommunicationPtr create(const common::ConnectParams &params) { return std::make_shared<UdpcComm>(params); }
	bool open() override { return false; }
	bool open(const string &host, unsigned short port) override;
	void close() override;
	void start(const ReadFunction &callback) override;
	bool write(const unsigned char *buffer, size_t size) override;
};

}

#endif // UDPCCOMM_H

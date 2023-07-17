#ifndef IPORT_H
#define IPORT_H

#include "common/types.h"
#include "common/devicetypes.h"

namespace comms
{

// Communication interface
class ICommunication
{
protected:
	uint32_t _baudrate = 0;
	common::ConnectParams _connectParams;

public:
	ICommunication(const common::ConnectParams &params) : _connectParams(params) {}
	virtual ~ICommunication() {}

public:
	const common::ConnectParams &connectParams() const { return _connectParams; }
	virtual bool Open() = 0; // Open with ConnectParams
	virtual bool Open(const string &host, unsigned short port) = 0;
	virtual void Close() = 0;
	virtual void Start(const ReadFunction &callback) = 0;
	virtual bool Write(const unsigned char *buffer, size_t size) = 0;
	virtual void SetBaudRate(uint32_t rate) { _baudrate = rate; }
};

typedef std::shared_ptr<ICommunication> CommunicationPtr;

}

#endif // IPORT_H

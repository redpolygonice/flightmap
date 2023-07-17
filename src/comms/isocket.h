#ifndef ISOCKET_H
#define ISOCKET_H

#include "common/types.h"

namespace comms
{

#define CONNECT_TIMEOUT 1000
#define CONNECT_COUNT 50

// Socket interface
class ISocket
{
public:
	virtual ~ISocket() {}

public:
	virtual void close() = 0;
	virtual bool isValid() const = 0;
	virtual bool connect(const string &host) = 0;
	virtual bool listen(unsigned short port) = 0;
	virtual int read(char *data, size_t length) = 0;
	virtual int write(const char *data, size_t length) = 0;
};

typedef std::shared_ptr<ISocket> SocketPtr;

}

#endif // ISOCKET_H

#pragma once

#include "common/types.h"
#include "isocket.h"

#ifdef WIN32
#include <winsock.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR (-1)
#endif

namespace comms
{

#define RCVTIMEOUT 10

/** UDP Socket class */
class UdpSocket : public ISocket
{
private:
	string _host;
	unsigned short _port;
	int _addrFamily;
	int _type;
	int _proto;

	SOCKET _socket;
	struct sockaddr_in _sockaddr;

public:
	UdpSocket(unsigned short port, int proto = IPPROTO_UDP, int type = SOCK_DGRAM, int addrFamily = AF_INET);
	virtual ~UdpSocket();

public:
	static SocketPtr create(int port) { return std::make_shared<UdpSocket>(port); }
	bool isValid() const override { return _socket != INVALID_SOCKET; }
	void close() override;
	bool connect(const string &host) override;
	bool listen(unsigned short port) override;
	int read(char *buffer, size_t length) override;
	int write(const char *buffer, size_t length) override;
	int write(const string &data);

private:
	bool initAddress(bool server);
};

}

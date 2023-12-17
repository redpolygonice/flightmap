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

/** TCP Socket class */
class TcpSocket : public ISocket
{
private:
	string _host;
	unsigned short _port;
	int _addrFamily;
	int _type;
	int _proto;
	bool _server;
	bool _active;

	SOCKET _socket;
	struct sockaddr_in _sockaddr;

	std::vector<SOCKET> _rsockets;
	struct sockaddr_in _rsockaddr;

public:
	TcpSocket(unsigned short port, int proto = IPPROTO_TCP, int type = SOCK_STREAM, int addrFamily = AF_INET);
	virtual ~TcpSocket();

public:
	static SocketPtr create(int port) { return std::make_shared<TcpSocket>(port); }
	bool isValid() const override { return _socket != INVALID_SOCKET; }
	void close() override;
	bool connect(const string &host) override;
	bool listen(unsigned short port) override;
	int read(char *buffer, size_t length) override;
	int write(const char *buffer, size_t length) override;

private:
	bool initAddress(bool server);
	void run();
	void closeSocket(SOCKET &socket);
};

}

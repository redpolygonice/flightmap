#include "tcpsocket.h"
#include "common/log.h"

namespace comms
{

TcpSocket::TcpSocket(unsigned short port, int proto, int type, int addrFamily)
	:_host("")
	,_port(port)
	,_addrFamily(addrFamily)
	,_type(type)
	,_proto(proto)
	,_server(false)
	,_active(false)
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	_socket = INVALID_SOCKET;
	memset(&_sockaddr, 0, sizeof(sockaddr_in));
}

TcpSocket::~TcpSocket()
{
	close();
}

bool TcpSocket::initAddress(bool server)
{
	memset(&_sockaddr, 0, sizeof(sockaddr_in));
	_sockaddr.sin_family = _addrFamily;
	_sockaddr.sin_port = htons(_port);
	if (server)
		_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		_sockaddr.sin_addr.s_addr = inet_addr(_host.c_str());

	if (_sockaddr.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent *pHostent = gethostbyname(_host.c_str());
		if (pHostent == NULL)
			return false;
		_sockaddr.sin_addr = *(struct in_addr *)pHostent->h_addr;
	}

	return true;
}

bool TcpSocket::connect(const string &host)
{
	_host = host;

	if (!initAddress(false))
	{
		common::log("Socket::initAddress error");
		return false;
	}

	_socket = socket(_addrFamily, _type, _proto);
	if (_socket == INVALID_SOCKET)
	{
		common::log("Socket::socket error");
		return false;
	}

	if (::connect(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) == SOCKET_ERROR)
	{
		common::log("Socket::connect error");
		return false;
	}

	return true;
}

bool TcpSocket::listen(unsigned short port)
{
	_port = port;
	_server = true;

	if (!initAddress(true))
	{
		common::log("Socket::initAddress error");
		return false;
	}

	_socket = socket(_addrFamily, _type, _proto);
	if (_socket == INVALID_SOCKET)
	{
		common::log("Socket::socket error");
		return false;
	}

#ifdef WIN32
	const char on = 1;
#else
	const int on = 1;
#endif
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == SOCKET_ERROR)
	{
		common::log("Socket::setsockopt SO_REUSEADDR error");
		return false;
	}

	if (bind(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) == SOCKET_ERROR)
	{
		common::log("Socket::bind error");
		return false;
	}

	if (::listen(_socket, 10) == SOCKET_ERROR)
	{
		common::log("Socket::listen error");
		return false;
	}

	_active = true;
	std::thread theThread([this]() { run(); });
	theThread.detach();

	return true;
}

void TcpSocket::run()
{
	while (_active)
	{
		struct sockaddr_in sockaddr;
#ifdef WIN32
		int addrlen = sizeof(sockaddr);
#else
		socklen_t addrlen = sizeof(sockaddr);
#endif
		SOCKET socket = accept(_socket, (struct sockaddr *)&sockaddr, &addrlen);
		if (socket == SOCKET_ERROR)
		{
			common::log("Socket::accept error");
			continue;
		}

		// Set option SO_KEEPALIVE
		const char on = 1;
		if (setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) == SOCKET_ERROR)
		{
			common::log("Socket::setsockopt SO_KEEPALIVE error");
		}

		// Set option SO_RCVTIMEO
		struct timeval tv;
		tv.tv_sec = RCVTIMEOUT;
		tv.tv_usec = 0;
		if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) == SOCKET_ERROR)
		{
			common::log("Socket::setsockopt SO_SNDTIMEO error");
		}

		_rsockets.push_back(socket);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void TcpSocket::closeSocket(SOCKET &socket)
{
	if (socket == INVALID_SOCKET)
		return;

#ifdef WIN32
	closesocket(_socket);
	WSACleanup();
#else
	shutdown(socket, SHUT_RDWR);
	//::close(socket);
#endif

	socket = INVALID_SOCKET;
}

void TcpSocket::close()
{
	_active = false;
	for (std::vector<SOCKET>::iterator it = _rsockets.begin(); it != _rsockets.end(); ++it)
		closeSocket(*it);
	_rsockets.clear();
	closeSocket(_socket);
}

int TcpSocket::read(char *buffer, size_t length)
{
	int result = -1;

	if (_server)
	{
		for (SOCKET socket : _rsockets)
		{
			result = recv(socket, buffer, length, 0);

			// If error we guess it disconnected or broken socket
			if (result == SOCKET_ERROR)
			{
				common::log("Read socket error");
				_rsockets.erase(std::remove(_rsockets.begin(), _rsockets.end(), socket), _rsockets.end());
				closeSocket(socket);
			}
		}
	}
	else
	{
		result = recv(_socket, buffer, length, 0);
		if (result == SOCKET_ERROR)
			return -1;
	}

	return result;
}

int TcpSocket::write(const char *buffer, size_t length)
{
	int result = -1;
	if (length == 0 || buffer == NULL) return 0;

	if (_server)
	{
		for (SOCKET socket : _rsockets)
		{
			result = send(socket, buffer, length, 0);

			// If error we guess it disconnected or broken socket
			if (result == SOCKET_ERROR)
			{
				common::log("Write socket error");
				_rsockets.erase(std::remove(_rsockets.begin(), _rsockets.end(), socket), _rsockets.end());
				closeSocket(socket);
			}
		}
	}
	else
	{
		result = send(_socket, buffer, length, 0);
		if (result == SOCKET_ERROR)
			return -1;
	}

	return result;
}

}

#include "udpsocket.h"
#include "common/log.h"

namespace comms
{

UdpSocket::UdpSocket(unsigned short port, int proto, int type, int addrFamily)
:_host("")
,_port(port)
,_addrFamily(addrFamily)
,_type(type)
,_proto(proto)
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	_socket = INVALID_SOCKET;
	memset(&_sockaddr, 0, sizeof(sockaddr_in));
}

UdpSocket::~UdpSocket()
{
	close();
}

bool UdpSocket::initAddress(bool server)
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

bool UdpSocket::connect(const string &host)
{
	_host = host;

	if (!initAddress(false))
	{
		LOGE("Socket::initAddress error!");
		return false;
	}

	_socket = socket(_addrFamily, _type, _proto);
	if (_socket == INVALID_SOCKET)
	{
		LOGE("Socket::socket error!");
		return false;
	}

	if (::connect(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) == SOCKET_ERROR)
	{
		LOGE("Socket::connect error!");
		return false;
	}

	return true;
}

bool UdpSocket::listen(unsigned short port)
{
	_port = port;

	if (!initAddress(true))
	{
		LOGE("Socket::initAddress error!");
		return false;
	}

	_socket = socket(_addrFamily, _type, _proto);
	if (_socket == INVALID_SOCKET)
	{
		LOGE("Socket::socket error!");
		return false;
	}

#ifdef WIN32
	const char on = 1;
#else
	const int on = 1;
#endif
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == SOCKET_ERROR)
	{
		LOGE("Socket::setsockopt SO_REUSEADDR error!");
		return false;
	}

	if (bind(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) == SOCKET_ERROR)
	{
		LOGE("Socket::bind error!");
		return false;
	}

	return true;
}

void UdpSocket::close()
{
	if (_socket == INVALID_SOCKET)
		return;

#ifdef WIN32
	closesocket(_socket);
	WSACleanup();
#else
	shutdown(_socket, SHUT_RDWR);
	//::close(_socket);
#endif

	_socket = INVALID_SOCKET;
}

int UdpSocket::read(char *buffer, size_t length)
{
#ifdef WIN32
		int addrlen = sizeof(sockaddr);
#else
		socklen_t addrlen = sizeof(sockaddr);
#endif
	int result = recvfrom(_socket, buffer, length, 0, (struct sockaddr *)&_sockaddr, &addrlen);
	if (result == SOCKET_ERROR)
	{
		LOGE("Read socket error!");
		return -1;
	}

	return result;
}

int UdpSocket::write(const char *buffer, size_t length)
{
	if (length == 0 || buffer == NULL) return 0;

	int result = sendto(_socket, buffer, length, 0, (const struct sockaddr *)&_sockaddr, sizeof(_sockaddr));
	if (result == SOCKET_ERROR)
	{
		LOGE("Write socket error!");
		return -1;
	}

	return result;
}

int UdpSocket::write(const std::string &data)
{
	return write(data.c_str(), data.size());
}

}

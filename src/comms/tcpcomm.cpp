#include "tcpcomm.h"
#include "comms/tcpsocket.h"
#include "common/log.h"
#include "common/common.h"
#include <cstdint>

namespace comms
{

TcpComm::TcpComm(const common::ConnectParams &params)
	: ICommunication(params)
	, _active(false)
	, _socket(nullptr)
	, _callback(nullptr)
{
}

TcpComm::~TcpComm()
{
}

bool TcpComm::Open()
{
	return Open(_connectParams.host, _connectParams.port);
}

bool TcpComm::Open(const std::string &host, unsigned short port)
{
	_socket = TcpSocket::create(port);

	bool connected = false;
	int count = CONNECT_COUNT;
	while (count-- > 0)
	{
		if (_socket->connect(host))
		{
			connected = true;
			break;
		}

		common::Sleep(CONNECT_TIMEOUT);
	}

	if (connected) {
		LOG("TCP connect OK");
	}
	else {
		LOGW("TCP connection failed!");
	}

	return connected;
}

void TcpComm::Close()
{
	_active = false;
	if (_thread.joinable())
		_thread.join();

	_socket->close();
	_socket.reset();
	_socket = nullptr;
}

void TcpComm::Start(const ReadFunction &callback)
{
	_callback = callback;
	_active = true;
	_thread = std::thread([this]() { run(); });
}

void TcpComm::run()
{
	uint8_t buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	while (_active)
	{
		int size = _socket->read(reinterpret_cast<char *>(buffer), BUFFER_SIZE);
		if (size > 0)
		{
			if (_callback != nullptr)
			{
				ByteArray data(buffer, buffer + size);
				_callback(data);
			}
		}

		common::Sleep(10);
	}
}

bool TcpComm::Write(const unsigned char *buffer, size_t size)
{
	if (_socket->write(reinterpret_cast<const char *>(buffer), size) > 0)
		return true;
	return false;
}

}

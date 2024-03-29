#include "udpscomm.h"
#include "udpsocket.h"
#include "common/log.h"
#include "common/common.h"

namespace comms
{

UdpsComm::UdpsComm(const common::ConnectParams &params)
	: ICommunication(params)
	, _active(false)
	, _socket(nullptr)
	, _callback(nullptr)
{
}

UdpsComm::~UdpsComm()
{
}

bool UdpsComm::Open()
{
	return Open(_connectParams.host, _connectParams.port);
}

bool UdpsComm::Open(const std::string &host, unsigned short port)
{
	_socket = UdpSocket::create(port);
	if (!_socket->listen(port))
	{
		LOGW("[UdpsComm] Can't connect to socket!");
		return false;
	}

	return true;
}

void UdpsComm::Close()
{
	_active = false;
	if (_thread.joinable())
		_thread.join();

	_socket->close();
	_socket.reset();
	_socket = nullptr;
}

void UdpsComm::Start(const ReadFunction &callback)
{
	_callback = callback;
	_active = true;
	_thread = std::thread([this]() { run(); });
}

void UdpsComm::run()
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

bool UdpsComm::Write(const unsigned char *buffer, size_t size)
{
	if (_socket->write(reinterpret_cast<const char *>(buffer), size) > 0)
		return true;
	return false;
}

}

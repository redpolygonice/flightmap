#include "mockcomm.h"
#include "common/log.h"
#include "common/common.h"
#include "main/broker.h"

#include <cstddef>
#include <cstdint>

namespace comms
{

MockComm::MockComm(const common::ConnectParams &params)
	: ICommunication(params)
	, _active(false)
	, _callback(nullptr)
{
}

MockComm::~MockComm()
{
}

bool MockComm::Open()
{
	return true;
}

bool MockComm::Open(const std::string &host, unsigned short port)
{
	return true;
}

void MockComm::Close()
{
	_active = false;
	_simulator.Stop();
}

void MockComm::Start(const ReadFunction &callback)
{
	_active = true;
	_callback = callback;
	_simulator.Start(_callback);

//	_thread = std::thread([this]() {
//		while (_active)
//		{
//			ByteArray data;
//			if (_simulator.Read(data))
//			{
//				if (_callback != nullptr)
//					_callback(data);
//			}

//			common::Sleep(10);
//		}
//	});
}

bool MockComm::Write(const unsigned char *buffer, size_t size)
{
	ByteArray data(buffer, buffer + size);
	_simulator.Write(data);
	return true;
}

}

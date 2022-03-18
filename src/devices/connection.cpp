#include "connection.h"

namespace device
{

Connection::Connection(const device::DevicePtr &device)
	: _device(device)
	, _number(-1)
	, _active(false)
{
}

Connection::~Connection()
{
	stop();
}

void Connection::stop()
{
	_device->stop();
	_device.reset();
	_number = -1;
	_active = false;
}

}

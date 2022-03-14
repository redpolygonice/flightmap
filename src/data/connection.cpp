#include "connection.h"

namespace data
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

ConnectionPtr ConnectionList::active() const
{
	ConnectionListConstIterator it = std::find_if(_data.begin(), _data.end(), [this](const ConnectionPtr &connection) {
		return (connection->isActive() == true);
	});

	if (it != _data.end())
		return *it;
	return nullptr;
}

void ConnectionList::setActive(int number)
{
	for (const ConnectionPtr &conn : _data)
		conn->_active = false;

	ConnectionPtr connection = findByNumber(number);
	if (connection != nullptr)
		connection->_active = true;
}

void ConnectionList::add(const device::DevicePtr &device)
{
	static int number = 0;
	ConnectionPtr connection = Connection::create(device);
	connection->_number = ++number;
	connection->_active = true;

	for (const ConnectionPtr &conn : _data)
		conn->_active = false;

	_data.push_back(connection);
}

void ConnectionList::remove(const device::DevicePtr &device)
{
	ConnectionListIterator it = std::find_if(_data.begin(), _data.end(), [device](const ConnectionPtr &connection) {
		return (connection->device() == device ? true : false);
	});

	if (it != _data.end())
		_data.erase(std::remove(_data.begin(), _data.end(), *it), _data.end());
}

void ConnectionList::remove(int number)
{
	ConnectionListIterator it = std::find_if(_data.begin(), _data.end(), [number](const ConnectionPtr &connection) {
		return (connection->number() == number ? true : false);
	});

	if (it != _data.end())
		_data.erase(std::remove(_data.begin(), _data.end(), *it), _data.end());
}

void ConnectionList::removeActive()
{
	remove(active()->device());
}

void ConnectionList::removeAll()
{
	for (const ConnectionPtr &connection : _data)
		connection->stop();
	_data.clear();
}

ConnectionPtr ConnectionList::findByNumber(int number) const
{
	ConnectionListConstIterator it = std::find_if(_data.begin(), _data.end(), [number](const ConnectionPtr &connection) {
		return (connection->number() == number ? true : false);
	});

	if (it != _data.end())
		return *it;
	return nullptr;
}

device::DevicePtr ConnectionList::findDeviceByNumber(int number) const
{
	ConnectionPtr connection = findByNumber(number);
	if (connection != nullptr)
		return connection->device();
	return nullptr;
}

}

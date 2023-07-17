#include "devicemanager.h"

namespace device
{

ConnectionPtr DeviceManager::Active() const
{
	ConnectionListConstIterator it = std::find_if(_data.begin(), _data.end(), [this](const ConnectionPtr &connection) {
		return (connection->IsActive() == true);
	});

	if (it != _data.end())
		return *it;
	return nullptr;
}

void DeviceManager::SetActive(int number)
{
	for (const ConnectionPtr &conn : _data)
		conn->_active = false;

	ConnectionPtr connection = FindByNumber(number);
	if (connection != nullptr)
		connection->_active = true;
}

void DeviceManager::Add(const device::DevicePtr &device)
{
	static int number = 0;
	ConnectionPtr connection = Connection::Create(device);
	connection->_number = ++number;
	connection->_active = true;

	for (const ConnectionPtr &conn : _data)
		conn->_active = false;

	_data.push_back(connection);
}

void DeviceManager::Remove(const device::DevicePtr &device)
{
	ConnectionListIterator it = std::find_if(_data.begin(), _data.end(), [device](const ConnectionPtr &connection) {
		return (connection->GetDevice() == device ? true : false);
	});

	if (it != _data.end())
		_data.erase(std::remove(_data.begin(), _data.end(), *it), _data.end());
}

void DeviceManager::Remove(int number)
{
	ConnectionListIterator it = std::find_if(_data.begin(), _data.end(), [number](const ConnectionPtr &connection) {
		return (connection->Number() == number ? true : false);
	});

	if (it != _data.end())
		_data.erase(std::remove(_data.begin(), _data.end(), *it), _data.end());
}

void DeviceManager::RemoveActive()
{
	Remove(Active()->GetDevice());
}

void DeviceManager::RemoveAll()
{
	for (const ConnectionPtr &connection : _data)
		connection->Stop();
	_data.clear();
}

ConnectionPtr DeviceManager::FindByNumber(int number) const
{
	ConnectionListConstIterator it = std::find_if(_data.begin(), _data.end(), [number](const ConnectionPtr &connection) {
		return (connection->Number() == number ? true : false);
	});

	if (it != _data.end())
		return *it;
	return nullptr;
}

device::DevicePtr DeviceManager::FindDeviceByNumber(int number) const
{
	ConnectionPtr connection = FindByNumber(number);
	if (connection != nullptr)
		return connection->GetDevice();
	return nullptr;
}

}

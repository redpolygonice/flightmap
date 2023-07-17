#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "common/types.h"
#include "idevice.h"
#include "connection.h"
#include <cstddef>

namespace device
{

class DeviceManager;
typedef std::shared_ptr<DeviceManager> DeviceManagerPtr;

// Device manager class
class DeviceManager
{
private:
	ConnectionList _data;

public:
	DeviceManager() {}
	~DeviceManager() {}

	DeviceManager(const DeviceManager&) = delete;
	DeviceManager(DeviceManager&&) = delete;
	DeviceManager& operator=(const DeviceManager&) = delete;
	DeviceManager& operator=(DeviceManager&&) = delete;

public:
	static DeviceManagerPtr Create() { return std::make_shared<DeviceManager>(); }
	size_t Count() const { return _data.size(); }
	ConnectionPtr Active() const;
	void SetActive(int number);
	void Add(const device::DevicePtr &device);
	void Remove(const device::DevicePtr &device);
	void Remove(int number);
	void RemoveActive();
	void RemoveAll();
	ConnectionPtr FindByNumber(int number) const;
	device::DevicePtr FindDeviceByNumber(int number) const;
};

}

#endif // DEVICEMANAGER_H

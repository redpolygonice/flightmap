#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "common/types.h"
#include "idevice.h"
#include "connection.h"

namespace device
{

class ConnectionManager;
typedef std::shared_ptr<ConnectionManager> ConnectionManagerPtr;

class ConnectionManager
{
private:
	ConnectionList _data;

public:
	ConnectionManager() {}
	~ConnectionManager() {}

public:
	static ConnectionManagerPtr create() { return std::make_shared<ConnectionManager>(); }
	ConnectionPtr active() const;
	void setActive(int number);
	void add(const device::DevicePtr &device);
	void remove(const device::DevicePtr &device);
	void remove(int number);
	void removeActive();
	void removeAll();
	ConnectionPtr findByNumber(int number) const;
	device::DevicePtr findDeviceByNumber(int number) const;
};



}

#endif // CONNECTIONMANAGER_H

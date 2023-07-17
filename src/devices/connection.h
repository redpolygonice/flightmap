#ifndef CONNECTION_H
#define CONNECTION_H

#include "common/types.h"
#include "idevice.h"
#include "comms/icommunication.h"

namespace device
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

typedef std::vector<ConnectionPtr> ConnectionList;
typedef std::vector<ConnectionPtr>::iterator ConnectionListIterator;
typedef std::vector<ConnectionPtr>::const_iterator ConnectionListConstIterator;

// Connection object
class Connection
{
	friend class DeviceManager;

private:
	device::DevicePtr _device;
	int _number;
	bool _active;

public:
	Connection(const device::DevicePtr &device);
	~Connection();

public:
	static ConnectionPtr Create(const device::DevicePtr &device) { return std::make_shared<Connection>(device); }
	void Stop();
	device::DevicePtr GetDevice() const { return _device; }
	int Number() const { return _number; }
	bool IsActive() const { return _active; }
};

}

#endif // CONNECTION_H

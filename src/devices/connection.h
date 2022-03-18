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
	friend class ConnectionManager;

private:
	device::DevicePtr _device;
	int _number;
	bool _active;

public:
	Connection(const device::DevicePtr &device);
	~Connection();

public:
	static ConnectionPtr create(const device::DevicePtr &device) { return std::make_shared<Connection>(device); }
	void stop();
	device::DevicePtr device() const { return _device; }
	int number() const { return _number; }
	bool isActive() const { return _active; }
};

}

#endif // CONNECTION_H
